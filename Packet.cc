#include "Packet.hh"
#include "Army.hh" 
#include "Tile.hh" 
#include <cmath> 

double Packet::speed = 0.0001; 
vector<Packet*> Packet::allPackets; 

Packet::Packet () 
  : target(0) 
  , tile(0)
{
  allPackets.push_back(this);
}

Packet::~Packet () {
  for (unsigned int i = 0; i < allPackets.size(); ++i) {
    if (allPackets[i] != this) continue;
    allPackets[i] = allPackets.back();
    break;
  }
  allPackets.pop_back(); 
}

bool Packet::update (int elapsedTime) {
  if (!target) target = Army::getClosestFriendly(position, player1);
  tile = Tile::getClosest(position, tile);
 
  double totalInfluence = 0; 
  for (Tile::VertIter vert = tile->startv(); vert != tile->finalv(); ++vert) {
    totalInfluence += (*vert)->myControl(player1); 
  }
  totalInfluence *= 0.25; // Max influence is 4
  if (totalInfluence < 0.1) {
    size /= 2;
    player1 = !player1; 
    target = 0;
    return false; 
  }
  else {
    size = (int) floor(size * totalInfluence + 0.5); 
    if (1 > size) return true; 
  }

  double distance = position.distance(target->position);
  if (speed*elapsedTime > distance) {
    target->supplies += size;
    return true; 
  }

  Army* closestEnemy = Army::getClosestEnemy(position, player1); 
  double distanceSq = closestEnemy->position.distanceSq(position); 
  if (distanceSq < 25) {
    closestEnemy->supplies += size/2; 
    return true; 
  }

  point dirVector = target->position;
  dirVector -= position;
  dirVector.normalise(); 
  dirVector *= speed*elapsedTime; 
  position += dirVector; 
  
  return false; 
}
