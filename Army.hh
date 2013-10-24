#ifndef ARMY_HH
#define ARMY_HH
#include <vector>
#include "utils.hh" 

using namespace std;

struct Army {
  Army ();
  ~Army (); 

  double supplies1; 
  double supplies2; 
  point position; 
  point advance; 

  double fight (int elapsedTime); 
  static Army* getClosest (const point& pt); 
  typedef vector<Army*>::iterator Iter;
  static Iter begin () {return allArmies.begin();}
  static Iter final () {return allArmies.end();} 

private:
  static double speed; // In pixels per microsecond
  static double maxAdvance; 
  static vector<Army*> allArmies; 
};


#endif 
