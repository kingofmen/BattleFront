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
  double radius = min(0.2*supplies, 30.0); 
  double radiusSq = radius*radius;
  double totalCombat = 0; 
  for (Iter army = start(); army != final(); ++army) {
    if ((*army)->player == player) continue; 
    point line = (*army)->position; // Points from us to them 
    line -= position;
    double distanceSq = line.lengthSq(); 
    if (distanceSq > radiusSq) continue;
    line.normalise(); 
    double pushThem = supplies / (1 + (*army)->supplies);
    totalCombat += (*army)->supplies / (0.001 + sqrt(distanceSq / radiusSq)); 
    (*army)->move(line*pushThem);; 
    pushThem = (-1.0) / pushThem; 
    move(line*pushThem); 
  }

  // Normalisation: A unit fighting an equal-sized unit at a 
  // distance of half its combat radius runs down half its
  // supplies in five seconds. Not fighting, it runs down half
  // its supplies in a minute. 
  totalCombat /= supplies;
  supplies *= pow(0.5, elapsedTime*(totalCombat*2e-7 + 1.667e-8)); 
  if (supplies < 1) supplies = 1; 
}

void Army::influence (int elapsedTime) {
  for (Tile::VertIter vert = tile->startv(); vert != tile->finalv(); ++vert) {
    (*vert)->influence(elapsedTime, supplies, position, player); 
  }
}

void Army::move (const point& amount) {
  position += amount;
  if (position.x() < 1)    position.x() = 1;
  if (position.x() > 1079) position.x() = 1079;

  if (position.y() < 1)   position.y() = 1;
  if (position.y() > 769) position.y() = 769;
}

bool Army::testForEnemy (Vertex const* const vert, point& direction, double& totalWeight) {
  double enemyControl = vert->enemyControl(player);
  if (enemyControl <= 0.01) return false;
  point vdir = vert->position;
  vdir -= position;
  vdir.normalise(); 
  vdir *= enemyControl;
  totalWeight += enemyControl;
  direction += vdir;
  return true; 
}

void Army::advance (int elapsedTime) {
  // Are there enemy vertices in the local tile? 
  point direction;
  bool goodEnough = false; 
  double totalWeight = 0; 
  for (Tile::VertIter vert = tile->startv(); vert != tile->finalv(); ++vert) {
    goodEnough |= testForEnemy((*vert), direction, totalWeight);
  }

  if (goodEnough) {
    direction /= totalWeight; 
    direction.normalise(); 
    direction *= speed*elapsedTime; 
    move(direction);
    return; 
  }

  // Check the immediate neighbourhood; also look for enemy armies
  for (Tile::Iter t = tile->startn(); t != tile->finaln(); ++t) {
    for (Tile::VertIter vert = (*t)->startv(); vert != (*t)->finalv(); ++vert) {
      goodEnough |= testForEnemy((*vert), direction, totalWeight);
    }
  }
  
  for (Iter army = start(); army != final(); ++army) {
    if ((*army)->player == player) continue; 
    point line = (*army)->position; // Points from us to them 
    line -= position;
    double distanceSq = line.lengthSq(); 
    if (distanceSq > 1e4) continue;
    goodEnough = true; 
    line.normalise(); 
    direction += line;
    totalWeight += 1;
  }
  
  if (goodEnough) {
    direction /= totalWeight;  
    direction.normalise();
    direction *= speed*elapsedTime; 
    move(direction);
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

Army* Army::getClosestFriendly (const point& pt, bool player) {
  Army* target = 0; 
  double distance = 1e20; 
  for (unsigned int i = 0; i < allArmies.size(); ++i) {
    if (allArmies[i]->player != player) continue;
    double currDist = pt.distance(allArmies[i]->position);
    if (currDist > distance) continue;
    distance = currDist;
    target = allArmies[i]; 
  }
  return target; 
}
