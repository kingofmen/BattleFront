#ifndef ARMY_HH
#define ARMY_HH
#include <vector>
#include "utils.hh" 
#include "Tile.hh" 

using namespace std;

struct Army {
  Army ();
  ~Army (); 

  double supplies; 
  point position; 
  bool player; 


  void fight (int elapsedTime); 
  void influence (int elapsedTime); 
  void advance (int elapsedTime); 

  static Army* getClosest (const point& pt); 
  static Army* getClosestFriendly (const point& pt, bool player); 
  typedef vector<Army*>::iterator Iter;
  static Iter start () {return allArmies.begin();}
  static Iter final () {return allArmies.end();} 

private:
  Tile* tile; 

  void move (const point& amount); 
  bool testForEnemy (Vertex const* const vert, point& direction, double& totalWeight);

  static double speed; // In pixels per microsecond
  static double maxAdvance; 
  static vector<Army*> allArmies; 
};


#endif 
