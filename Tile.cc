#include "Tile.hh"
#include <cmath> 

vector<Tile*> Tile::allTiles; 

Vertex::Vertex (point p, double c) 
  : position(p)
  , playerControl(c)
{}

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

void Vertex::influence (int elapsedTime, double armySize, const point& pos, bool player) {
  // Normalisation: A unit of size 100, in the middle of a tile, unopposed,
  // should take over all four vertices in one second. 

  const double normalTime = 1e-6;
  const double normalDist = 5*sqrt(2); 
  const double normalSize = 0.01; 
  
  double distance = pos.distance(position);
  double changeInControl = (elapsedTime*normalTime) * (normalDist/(1+distance)) * (armySize*normalSize); 

  if (!player) changeInControl *= -1;
  playerControl += changeInControl; 

}

void Vertex::renormalise () {
  if (playerControl > 1) playerControl = 1;
  else if (playerControl < 0) playerControl = 0;
}
