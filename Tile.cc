#include "Tile.hh"
#include <cmath> 
#include <cassert> 

vector<Tile*> Tile::allTiles; 
vector<Vertex*> Vertex::allVertices; 

Vertex::Vertex (point p, bool c, double t) 
  : position(p)
  , player(c)
  , troops(t)
  , inFlux(0) 
  , north(0)
  , east(0)
  , south(0)
  , west(0)
{
  neighbours.resize(4); 
  allVertices.push_back(this); 
}

void Vertex::setNeighbour (Direction card, Vertex* n) {
  switch (card) {
  case North: north = n; break;
  case East : east  = n; break;
  case South: south = n; break;
  case West : west  = n; break;
  case NumDirections:
  default:
    assert(false); 
  }
  neighbours[card] = n; 
}

void Vertex::fight (int elapsedTime) {
  for (Iter v = start(); v != final(); ++v) {
    (*v)->enemyTroops = 0;
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player == (*v)->player) continue;
      (*v)->enemyTroops += (*n)->troops;
    }
  }

  for (Iter v = start(); v != final(); ++v) {
    // Casualties proportional to square of enemy troops. 
    (*v)->troops -= 0.00000001 * pow((*v)->enemyTroops, 2) * elapsedTime;
    if ((*v)->troops < 0) (*v)->troops = 0; 
  }
}

void Vertex::move (int elapsedTime) {
  // Count enemy troops
  for (Iter v = start(); v != final(); ++v) {
    (*v)->inFlux = 0;
    (*v)->enemyTroops = 0;
    if (1 >= (*v)->troops) continue; 
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player == (*v)->player) continue;
      (*v)->enemyTroops += (*n)->troops;
    }
  }

  // Retreat everywhere that's outnumbered by 4 to 1 or more 
  for (Iter v = start(); v != final(); ++v) {
    if ((*v)->troops > 4 * (*v)->enemyTroops) continue; 

    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player != (*v)->player) continue;
      if ((*n)->troops < 4 * (*n)->enemyTroops) continue;
      (*n)->troops += (*v)->troops;
      break; 
    }
    (*v)->troops = 0; 
  }

  // Count troops after retreat
  for (Iter v = start(); v != final(); ++v) {
    (*v)->inFlux = 0;
    (*v)->enemyTroops = 0;
    if (1 >= (*v)->troops) continue; 
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player == (*v)->player) continue;
      (*v)->enemyTroops += (*n)->troops;
    }
  }

  // Take over empty enemy vertices where possible
  for (Iter v = start(); v != final(); ++v) {
    if ((*v)->troops < 4 * (*v)->enemyTroops) continue; 

    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player == (*v)->player) continue;
      if ((*n)->troops > 0.001) continue;
      (*n)->player = (*v)->player;
      (*n)->troops = 0.5*(*v)->troops;
      (*v)->troops *= 0.5;
      break; 
    }
  }

  // Again with counting enemy troops!
  for (Iter v = start(); v != final(); ++v) {
    (*v)->inFlux = 0;
    (*v)->enemyTroops = 0;
    if (1 >= (*v)->troops) continue; 
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player == (*v)->player) continue;
      (*v)->enemyTroops += (*n)->troops;
    }
  }

  // Reinforce friendly vertices where possible
  for (Iter v = start(); v != final(); ++v) {
    if ((*v)->troops < 1) continue; 
    if ((*v)->troops < 2 * (*v)->enemyTroops) continue; 
	
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player != (*v)->player) continue;
      if ((*n)->troops > (*v)->troops) continue;
      double diff = (*v)->troops - (*n)->troops;
      diff *= 0.5;
      (*n)->troops += diff;
      (*v)->troops -= diff; 
    }
  }
  
}

Vertex* Vertex::getClosestFighting (const point& pos, bool player) {
  double distance = 1e20;
  Vertex* ret = 0; 
  for (Iter v = start(); v != final(); ++v) {
    if ((*v)->player != player) continue;

    bool fighting = false; 
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue;
      if ((*n)->player == (*v)->player) continue;
      fighting = true;
      break;
    }
    if (!fighting) continue;
    double currDist = (*v)->position.distance(pos);
    if (currDist > distance) continue;
    ret = (*v);
    distance = currDist; 
  }
  return ret; 
}

Tile::Tile (Vertex* dl, Vertex* dr, Vertex* ur, Vertex* ul) {
  corners.resize(4); 
  corners[0] = dl;
  corners[1] = dr;
  corners[2] = ur;
  corners[3] = ul;
  
  // Find neighbours
  for (Iter cand = start(); cand != final(); ++cand) {
    // Yay, loop unroll ftw! 
    if ((corners[0] == (*cand)->corners[1]) ||
	(corners[0] == (*cand)->corners[2]) ||
	(corners[0] == (*cand)->corners[3]) ||
	(corners[1] == (*cand)->corners[0]) ||
	(corners[1] == (*cand)->corners[2]) ||
	(corners[1] == (*cand)->corners[3]) ||
	(corners[2] == (*cand)->corners[0]) ||
	(corners[2] == (*cand)->corners[1]) ||
	(corners[2] == (*cand)->corners[3]) ||
	(corners[3] == (*cand)->corners[0]) ||
	(corners[3] == (*cand)->corners[1]) ||
	(corners[3] == (*cand)->corners[2])) {

      neighbours.push_back(*cand);
      (*cand)->neighbours.push_back(this); 
    }
  }

  allTiles.push_back(this); 
}

Tile* Tile::getClosest (point position, Tile* previous) {
  // Check whether we're still internal to previous, 
  // to avoid loop over everything.
  if (previous) {
    if ((position.x() >= previous->corners[0]->position.x()) &&
	(position.x() <= previous->corners[1]->position.x()) &&
	(position.y() >= previous->corners[0]->position.y()) &&
	(position.y() <= previous->corners[2]->position.y())) return previous; 
  }

  for (Iter t = start(); t != final(); ++t) {
    // Tiles are squares, so no need for full is-within-polygon test. 
    if (position.x() < (*t)->corners[0]->position.x()) continue;
    if (position.x() > (*t)->corners[1]->position.x()) continue;
    if (position.y() < (*t)->corners[0]->position.y()) continue;
    if (position.y() > (*t)->corners[2]->position.y()) continue;
    return (*t); 
  }
  return 0; 
}

double Vertex::influence (int elapsedTime, double armySize, const point& pos, bool player) {
  // Normalisation: A unit of size 100, in the middle of a tile, unopposed,
  // should take over all four vertices in one second. 

  const double normalTime = 1e-6;
  const double normalDist = 5*sqrt(2); 
  const double normalSize = 0.01; 
  
  double distance = pos.distance(position);
  double changeInControl = (elapsedTime*normalTime) * (normalDist/(1+distance)) * (armySize*normalSize); 
  double ret = enemyControl(player); 

  if (!player) changeInControl *= -1;
  playerControl += changeInControl; 
  return ret; 
}

void Vertex::renormalise () {
  if (playerControl > 0.75) playerControl += 0.01;
  else if (playerControl < 0.25) playerControl -= 0.01; 
  if (playerControl > 1) playerControl = 1;
  else if (playerControl < 0) playerControl = 0;
}

double Tile::avgControl (bool player) {
  double ret = 0; 
  for (VertIter v = startv(); v != finalv(); ++v) ret += (*v)->myControl(player);
  ret *= 0.25;
  return ret; 
}

void Tile::spreadInfluence (int elapsedTime) {
  // Normalisation: Halfway towards equilibration every two seconds

  for (Iter tile = start(); tile != final(); ++tile) {
    for (VertIter v = (*tile)->startv(); v != (*tile)->finalv(); ++v) {
      (*v)->inFlux = 0;
    }
  }

  for (Iter tile = start(); tile != final(); ++tile) {
    double avg = 0;
    for (VertIter v = (*tile)->startv(); v != (*tile)->finalv(); ++v) {
      avg += (*v)->playerControl; 
    }
    avg *= 0.25; 
    for (VertIter v = (*tile)->startv(); v != (*tile)->finalv(); ++v) {
      double diff = avg - (*v)->playerControl; 
      diff *= (1 - pow(0.5, elapsedTime * 5.0e-7)); 
      (*v)->inFlux += diff; 
    }
  }

  for (Iter tile = start(); tile != final(); ++tile) {
    for (VertIter v = (*tile)->startv(); v != (*tile)->finalv(); ++v) {
      (*v)->playerControl += (*v)->inFlux; 
    }
  }

}
