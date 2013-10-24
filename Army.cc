#include <cmath>
#include <algorithm> 
#include "Army.hh" 

double Army::speed = 0.00005;
double Army::maxAdvance = 3; 
vector<Army*> Army::allArmies; 

Army::Army () 
  : position(0, 0) 
{
  allArmies.push_back(this); 
} 

Army::~Army () {
  vector<Army*>::iterator me = find(allArmies.begin(), allArmies.end(), this);
  allArmies.erase(me); 
}

double Army::fight (int elapsedTime) {
  // Returns the advance made, in pixels. 
  // Negative if the advance is by the AI. 
  double advance = (supplies1 > supplies2 ? (supplies1 / supplies2) : (supplies2 / supplies1));
  if (fabs(advance) > 1.1) {
    advance = max(maxAdvance, sqrt(advance));
    advance *= speed;
    advance *= elapsedTime; 
    if (advance > maxAdvance) advance = maxAdvance; 
    if (supplies1 > supplies2) supplies1 *= 0.95;
    else {
      supplies2 *= 0.95;
      advance   *= -1;
    }
  }
  else advance = 0; 
  supplies1 *= 0.90;
  supplies2 *= 0.90;

  supplies1 = max(supplies1, 50.0);
  supplies2 = max(supplies2, 50.0);
  return advance; 
}

Army* Army::getClosest (const point& pt) {
  Army* target = allArmies[0];
  double distance = pt.distance(target->position);
  for (unsigned int i = 1; i < allArmies.size(); ++i) {
    double currDist = pt.distance(allArmies[i]->position);
    if (currDist > distance) continue;
    distance = currDist;
    target = allArmies[i]; 
  }
  return target; 
}
