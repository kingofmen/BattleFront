#ifndef TILE_HH
#define TILE_HH

#include "utils.hh" 
#include <vector> 

struct Vertex : public Iterable<Vertex> {
  friend class StaticInitialiser; 
  friend void drawTiles (); 
  friend class Tile; 
  enum Direction {North = 0, East, South, West, NumDirections}; 
  typedef vector<Vertex*>::iterator Iter;

  Vertex (point p, bool c, double t);

  point position;
  bool player;
  double troops; 
  bool debug; 

  double enemyControl (bool p) const {return (p == player) ? 0 : 1;}
  double influence (int elapsedTime, double armySize, const point& pos, bool player);
  int getFrontDistance () const {return frontDistance;} 
  double myControl    (bool p) const {return (p == player) ? 1 : 0;} 
  void renormalise();
  void setNeighbour (Direction card, Vertex* n); 
  Iter startn () {return neighbours.begin();}
  Iter finaln () {return neighbours.end();}

  static void attrite (int elapsedTime); 
  static void fight (int elapsedTime); 
  static Vertex* getClosestFighting (const point& pos, bool player); 
  static void move (int elapsedTime);  

private:
  double moved; 
  double enemyTroops; 
  bool flip;
  int frontDistance; 

  Vertex* north;
  Vertex* east;
  Vertex* south;
  Vertex* west; 
  vector<Vertex*> neighbours; 
  int cooldown; 

  static void countTroops (); 

  static double troopMoveRate;  // Units of troops per millisecond 
  static double coolDownFactor; // Milliseconds per troop received
  static double fightRate; 
  static double minimumGarrison; 
  static double attritionRate; 
};

struct Tile {
  Tile (Vertex* dl, Vertex* dr, Vertex* ur, Vertex* ul); 
  vector<Vertex*> corners;

  typedef Vertex::Iter VertIter; 
  typedef vector<Tile*>::iterator Iter;

  VertIter startv () {return corners.begin();}
  VertIter finalv () {return corners.end();} 
  Iter startn () {return neighbours.begin();}
  Iter finaln () {return neighbours.end();}
  double avgControl (bool player); 
  int frontDistance () const; 

  static Iter start () {return allTiles.begin();}
  static Iter final () {return allTiles.end();} 
  static Tile* getClosest (point position, Tile* previous); 

private:
  vector<Tile*> neighbours; 

  static vector<Tile*> allTiles; 
};

#endif 
