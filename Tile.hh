#ifndef TILE_HH
#define TILE_HH

#include "utils.hh" 
#include <vector> 

struct Vertex {
  Vertex (point p, double c);

  point position;
  double playerControl; 
};

struct Tile {
  Tile (Vertex* dl, Vertex* dr, Vertex* ur, Vertex* ul); 
  Vertex* corners[4];

  typedef vector<Tile*>::iterator Iter;
  static Iter start () {return allTiles.begin();}
  static Iter final () {return allTiles.end();} 

  static Tile* getClosest (point position); 

private:
  static vector<Tile*> allTiles; 
};

#endif 
