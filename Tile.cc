#include "Tile.hh"
#include <cmath> 
#include <cassert> 
#include <queue> 

vector<Tile*> Tile::allTiles; 
double Vertex::troopMoveRate = 0.001; 
double Vertex::fightRate = 0.0000001; 
double Vertex::minimumGarrison = 1; 
double Vertex::coolDownFactor = 10000;
double Vertex::attritionRate = 0.01;
double Vertex::artilleryScaleFactor = 1; 

Vertex::Vertex (point p, bool c, double t) 
  : Iterable<Vertex>(this)
  , position(p)
  , player(c)
  , troops(t)
  , debug(false)
  , moved(0) 
  , enemyTroops(0)
  , flip(false)
  , frontDistance(1000000000)
  , north(0)
  , east(0)
  , south(0)
  , west(0)
  , cooldown(0)
{
  neighbours.resize(4); 
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

void Vertex::attrite (int elapsedTime) {
  // attritionRate gives troops lost in one second. 

  double loss = exp(elapsedTime * 0.000001 * log(1 - attritionRate)); 
  for (Iter v = start(); v != final(); ++v) {
    (*v)->troops *= loss;
    if ((*v)->troops < minimumGarrison) (*v)->troops = minimumGarrison; 
  }
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
    // Casualties proportional to enemy troops.
    (*v)->m_LossesInfantry = (*v)->enemyTroops * fightRate * elapsedTime;
    // Modified by artillery.
    (*v)->m_LossesArtillery = (*v)->m_LossesInfantry * artilleryScaleFactor * (*v)->player ? (*v)->artillery.second : (*v)->artillery.first;
    double casualties = (*v)->m_LossesArtillery + (*v)->m_LossesInfantry; 
    if (1e-20 > casualties) continue; 
    if ((*v)->debug) std::cout << (*v)->position << " taking casualties " << casualties << std::endl;
    (*v)->troops -= casualties;
    if ((*v)->troops < 0) (*v)->troops = 0; 
  }
}

void Vertex::countTroops () {
  for (Iter v = start(); v != final(); ++v) {
    if (((*v)->debug) && ((*v)->moved > 0)) 
      std::cout << (*v)->position << " moves troops " << (*v)->moved << " to " 
		<< (*v)->troops + (*v)->moved << std::endl; 
    (*v)->troops += (*v)->moved;
    (*v)->moved = 0; 
    if ((*v)->troops < 0.01) (*v)->troops = 0; 
    if ((*v)->flip) (*v)->player = !(*v)->player; 
    (*v)->flip = false;  
  }

  queue<Vertex*> frontSpread;
  for (Iter v = start(); v != final(); ++v) {
    (*v)->enemyTroops = 0;
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player == (*v)->player) continue;
      (*v)->enemyTroops += (*n)->troops;
    }
    if (0.01 < (*v)->enemyTroops) {
      (*v)->frontDistance = 0;
      frontSpread.push(*v); 
    }
    else (*v)->frontDistance = 1000000000;
  }

  while (!frontSpread.empty()) {
    Vertex* curr = frontSpread.front(); 
    for (Iter n = curr->startn(); n != curr->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->frontDistance < curr->frontDistance + 2) continue;
      (*n)->frontDistance = curr->frontDistance + 1;
      frontSpread.push(*n); 
    }
    frontSpread.pop(); 
  }
}

void Vertex::move (int elapsedTime) {
  countTroops(); 
  double maxTroopsToMove = elapsedTime*troopMoveRate; // Rate of troops movement along one edge. 
  
  for (Iter v = start(); v != final(); ++v) {
    (*v)->cooldown -= elapsedTime;
    if (0 > (*v)->cooldown) (*v)->cooldown = 0;
    (*v)->artillery.first = (*v)->artillery.second = 0; 
  }

  // Retreat everywhere that's outnumbered by 4 to 1 or more 
  for (Iter v = start(); v != final(); ++v) {
    if (1e-20 > (*v)->enemyTroops) continue;
    if (4 * (*v)->troops > (*v)->enemyTroops) continue; 

    if ((*v)->debug) std::cout << (*v)->position << " retreating due to " << (*v)->troops << " " << (*v)->enemyTroops << "\n"; 

    (*v)->flip = true; 
    double totalRetreating = 0; 
    int placesToRetreat = 0;
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player != (*v)->player) continue;
      if ((*n)->troops < 4 * (*n)->enemyTroops) continue;
      if (0 < (*n)->cooldown) continue;
      totalRetreating += maxTroopsToMove; 
      placesToRetreat++;
    }
    if (0 == placesToRetreat) {
      (*v)->troops = 0; 
      (*v)->moved = 0; 
      continue;
    }
    
    if (totalRetreating > (*v)->troops) totalRetreating = (*v)->troops; 
    totalRetreating /= placesToRetreat;
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if (0 < (*n)->cooldown) continue;
      if ((*n)->player != (*v)->player) continue;
      if ((*n)->troops < 4 * (*n)->enemyTroops) continue;
      (*n)->moved += totalRetreating;
      (*n)->cooldown += (int) floor(totalRetreating * coolDownFactor + 0.5); 
      (*v)->moved -= totalRetreating;
    }
  }

  countTroops(); 
  
  // Reinforce friendly vertices where possible.
  // Fighting vertices have priority. 
  for (Iter v = start(); v != final(); ++v) {
    if (0 == (*v)->frontDistance) continue; 
    if (minimumGarrison >= (*v)->troops) continue; 
    if (0 < (*v)->cooldown) continue; 
    
    int placesToReinforce = 0;
    int lowestDistance = (*v)->frontDistance;
    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player != (*v)->player) continue;
      if (0 < (*n)->cooldown) continue;
      //if ((*n)->troops > 0.9999 * (*v)->troops) continue;
      if ((*n)->frontDistance > lowestDistance) continue;
      if ((*n)->frontDistance < lowestDistance) {
	placesToReinforce = 0;
	lowestDistance = (*n)->frontDistance;
      }

      placesToReinforce++; 
    }

    if (0 == placesToReinforce) continue; 
    double moving = ((*v)->troops - minimumGarrison) / placesToReinforce;
    if (moving > maxTroopsToMove) moving = maxTroopsToMove;

    for (Iter n = (*v)->startn(); n != (*v)->finaln(); ++n) {
      if (!(*n)) continue; 
      if ((*n)->player != (*v)->player) continue;
      if (0 < (*n)->cooldown) continue;
      //if ((*n)->troops > 0.999 * (*v)->troops) continue;
      if ((*n)->frontDistance > lowestDistance) continue;

      if (moving > (*v)->troops + (*v)->moved) moving = (*v)->troops + (*v)->moved; 
      if ((*v)->debug) std::cout << (*v)->position << " sending " << moving << " to " << (*n)->position << std::endl; 
      (*n)->moved += moving;
      (*n)->cooldown += (int) floor(moving * coolDownFactor + 0.5); 
      (*v)->moved -= moving; 
    }
  }
  
  countTroops();
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

double Tile::avgControl (bool player) {
  double ret = 0; 
  for (VertIter v = startv(); v != finalv(); ++v) ret += (*v)->myControl(player);
  ret *= 0.25;
  return ret; 
}

int Tile::frontDistance () const {
  // Returns distance of closest tile
  int ret = corners[0]->frontDistance;
  for (unsigned int i = 1; i < corners.size(); ++i) ret = min(ret, corners[i]->frontDistance);
  return ret; 
}
