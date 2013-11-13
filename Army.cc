#include <cmath>
#include <algorithm> 
#include "Army.hh" 

double Army::speed = 0.00005; 
double Army::minRadius = 10;
double Army::maxRadius = 30;
double Army::supplyRadius = 0.2; 
double Army::combatDecay = 2e-7;
double Army::generalDecay = 1.667e-8;
double Army::minSupply = 1; 
double Army::invFullCombat = 2; 
double Army::enemyControlFraction = 0.5;

vector<Army*> Army::allArmies; 

Army::Army () 
  : position(0, 0) 
  , tile(0) 
  , combat(0)
  , conquer(0)
  , debug(false) 
{
  allArmies.push_back(this); 
} 

Army::~Army () {
  vector<Army*>::iterator me = find(allArmies.begin(), allArmies.end(), this);
  allArmies.erase(me); 
}

double getCombatAmount (double theirSupply, double ourSupply, double radFrac) {
  ourSupply *= radFrac; 
  return (theirSupply / ourSupply);
}

void Army::fight (int elapsedTime) {
  tile = Tile::getClosest(position, tile);
  
  // Push enemy armies
  double radius = max(minRadius, min(supplyRadius*supplies, maxRadius)); 
  double radiusSq = radius*radius;
  for (Iter army = start(); army != final(); ++army) {
    if ((*army)->player == player) continue; 
    point line = (*army)->position; // Points from us to them 
    line -= position;
    double distanceSq = line.lengthSq(); 
    if (distanceSq > radiusSq) continue;
    line.normalise(); 
    double pushThem = supplies / (*army)->supplies;
    double currCombat = getCombatAmount((*army)->supplies, supplies, 0.001 + sqrt(distanceSq / radiusSq));
    combat += currCombat;
    (*army)->combat += currCombat; 
    (*army)->move(line*pushThem);; 
    pushThem = (-1.0) / pushThem; 
    move(line*pushThem); 
  }
}

void Army::updateSupplies (int elapsedTime) {
  // Normalisation: A unit fighting an equal-sized unit at a 
  // distance of half its combat radius runs down half its
  // supplies in five seconds. Not fighting, it runs down half
  // its supplies in a minute. 
  supplies -= supplies*(1 - pow(0.5, elapsedTime*(combat*combatDecay + generalDecay)));
  combat = 0; 
  conquer = 0; 
  if (supplies < minSupply) supplies = minSupply; 
}

void Army::influence (int elapsedTime) {
  // Combat normalisation: "Full" combat is
  // fighting an enemy of equal supplies at a 
  // distance of half our combat radius, making 0.5. 

  double combatFraction = 1 - combat*invFullCombat;
  if (combatFraction < 0) return;
  elapsedTime = (int) floor(elapsedTime * combatFraction + 0.5); 

  for (Tile::VertIter vert = tile->startv(); vert != tile->finalv(); ++vert) {
    conquer += (*vert)->influence(elapsedTime, supplies, position, player); 
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
  double combatFraction = 1 - combat*invFullCombat;
  if (combatFraction < 0) return;
  elapsedTime = (int) floor(elapsedTime * combatFraction + 0.5); 

  // Influence normalisation: No movement if the tile is half controlled
  // by the enemy. 
  double conquerFraction = 1 - (conquer * enemyControlFraction); 
  if (conquerFraction < 0) return;
  elapsedTime = (int) floor(elapsedTime * conquerFraction + 0.5); 

  point direction;
  bool goodEnough = false; 
  double totalWeight = 0; 

  // Move towards nearest enemy army
  Army* nearest = getClosestEnemy(position, player);
  if (nearest) {
    point line = nearest->position; // Points from us to them 
    line -= position;
    double distanceSq = line.lengthSq(); 
    if (distanceSq < 1e4) {
      goodEnough = true; 
      direction = line;
    }
  }

  if (goodEnough) {
    direction.normalise(); 
    direction *= speed*elapsedTime; 
    move(direction);
    return; 
  }

  // Are there enemy vertices in the local tile? 
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

Army* Army::getClosestEnemy (const point& pt, bool player) {
  Army* target = 0; 
  double distance = 1e20; 
  for (unsigned int i = 0; i < allArmies.size(); ++i) {
    if (allArmies[i]->player == player) continue;
    double currDist = pt.distance(allArmies[i]->position);
    if (currDist > distance) continue;
    distance = currDist;
    target = allArmies[i]; 
  }
  return target; 
}
