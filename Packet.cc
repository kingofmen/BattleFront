#include "Packet.hh"
#include "Tile.hh" 
#include <cmath> 

double Packet::speed = 0.0001; 
vector<Packet*> Packet::allPackets; 

Packet::Packet () 
  : tile(0)
  , target(0)
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
  tile = Tile::getClosest(position, tile);
  if ((!target) || (target->player != player1)) target = Vertex::getClosestFighting(position, player1);
  if (!target) return true; 


  point direction = (target->position - position);
  double distance = direction.length();
  if (distance < speed*elapsedTime) {
    target->troops += size; 
    return true; 
  }
  direction.normalise(); 
  direction *= (speed*elapsedTime);
  position += direction;

  return false; 
}
