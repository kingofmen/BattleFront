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

void Army::fight (int elapsedTime) {
  tile = Tile::getClosest(position, tile);
  
  // Push enemy armies
  double radius = 0.2*supplies; 
  double radiusSq = radius*radius;
  double totalCombat = 0; 
  for (Iter army = start(); army != final(); ++army) {
    if ((*army)->player == player) continue; 
    point line = (*army)->position; // Points from us to them 
    line -= position;
    double distanceSq = line.lengthSq(); 
    if (distanceSq > radiusSq) continue;
    line.normalise(); 
    double pushThem = supplies / (*army)->supplies;
    totalCombat += (*army)->supplies / distanceSq; 
    (*army)->position += line*pushThem; 
    pushThem = (-1.0) / pushThem; 
    position += line*pushThem; 
  }
}

void Army::influence (int elapsedTime) {
  for (Tile::VertIter vert = tile->startv(); vert != tile->finalv(); ++vert) {
    (*vert)->influence(elapsedTime, supplies, position, player); 
  }
}

bool Army::testForEnemy (Vertex const* const vert, point& direction) {
  double enemyControl = vert->enemyControl(player);
  if (enemyControl <= 0.01) return false;
  point vdir = vert->position;
  vdir -= position;
  vdir *= enemyControl;
  direction += vdir;
  return true; 
}

void Army::advance (int elapsedTime) {
  // Are there enemy vertices in the local tile? 
  point direction;
  bool goodEnough = false; 
  for (Tile::VertIter vert = tile->startv(); vert != tile->finalv(); ++vert) {
    goodEnough |= testForEnemy((*vert), direction);
  }

  if (goodEnough) {
    direction.normalise(); 
    direction *= speed*elapsedTime; 
    position += direction; 
    return; 
  }

  // Check the immediate neighbourhood
  for (Tile::Iter t = tile->startn(); t != tile->finaln(); ++t) {
    for (Tile::VertIter vert = (*t)->startv(); vert != (*t)->finalv(); ++vert) {
      goodEnough |= testForEnemy((*vert), direction);
    }
  }

  if (goodEnough) {
    direction.normalise(); 
    direction *= speed*elapsedTime; 
    position += direction; 
    return; 
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
