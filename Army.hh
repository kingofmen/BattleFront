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


  void update (int elapsedTime); 
  static Army* getClosest (const point& pt); 
  typedef vector<Army*>::iterator Iter;
  static Iter start () {return allArmies.begin();}
  static Iter final () {return allArmies.end();} 

private:
  Tile* tile; 

  static double speed; // In pixels per microsecond
  static double maxAdvance; 
  static vector<Army*> allArmies; 
};


#endif 
