#include "Tile.hh"

vector<Tile*> Tile::allTiles; 

Vertex::Vertex (point p, double c) 
  : position(p)
  , playerControl(c)
{}

Tile::Tile (Vertex* dl, Vertex* dr, Vertex* ur, Vertex* ul) {
  corners[0] = dl;
  corners[1] = dr;
  corners[2] = ur;
  corners[3] = ul;
  allTiles.push_back(this); 
}

Tile* Tile::getClosest (point position) {
  for (Iter t = start(); t != final(); ++t) {
    // Tiles are squares, so I can cheat on the is-within test.
    if (position.x() < (*t)->corners[0]->position.x()) continue;
    if (position.x() > (*t)->corners[1]->position.x()) continue;
    if (position.y() < (*t)->corners[0]->position.y()) continue;
    if (position.y() > (*t)->corners[2]->position.y()) continue;
    return (*t); 
  }
}
