#ifndef TILE_HH
#define TILE_HH

#include "utils.hh" 
#include <vector> 

struct Vertex {
  Vertex (point p, double c);

  point position;
  double playerControl; 

  void influence (int elapsedTime, double armySize, const point& pos, bool player);
  void renormalise();
  double myControl    (bool player) const {return  player ? playerControl : 1-playerControl;} 
  double enemyControl (bool player) const {return !player ? playerControl : 1-playerControl;} 
};

struct Tile {
  Tile (Vertex* dl, Vertex* dr, Vertex* ur, Vertex* ul); 
  vector<Vertex*> corners;

  typedef vector<Vertex*>::iterator VertIter; 
  VertIter startv () {return corners.begin();}
  VertIter finalv () {return corners.end();} 

  typedef vector<Tile*>::iterator Iter;
  static Iter start () {return allTiles.begin();}
  static Iter final () {return allTiles.end();} 

  Iter startn () {return neighbours.begin();}
  Iter finaln () {return neighbours.end();}

  static Tile* getClosest (point position, Tile* previous); 

private:
  vector<Tile*> neighbours; 

  static vector<Tile*> allTiles; 
};

#endif 
