#include <cmath>
#include <algorithm> 
#include "Army.hh" 

double Army::speed = 0.00005;
double Army::maxAdvance = 3; 
vector<Army*> Army::allArmies; 

Army::Army () 
  : position(0, 0) 
  , tile(0) 
{
  allArmies.push_back(this); 
} 

Army::~Army () {
  vector<Army*>::iterator me = find(allArmies.begin(), allArmies.end(), this);
  allArmies.erase(me); 
}

void Army::update (int elapsedTime) {
  if (!tile) tile = Tile::getClosest(position);
  
  double radius = 0.2*supplies; 
  double radiusSq = radius*radius;
  for (Iter army = start(); army != final(); ++army) {
    if ((*army)->player == player) continue; 
    point line = (*army)->position; // Points from us to them 
    line -= position;
    double distanceSq = line.lengthSq(); 
    if (distanceSq > radiusSq) continue;
    line.normalise(); 
    double pushThem = supplies / (*army)->supplies;
    (*army)->position += line*pushThem; 
    pushThem = (-1.0) / pushThem; 
    position += line*pushThem; 
  }

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
