#include "Packet.hh"
#include "Army.hh" 

double Packet::speed = 0.0001; 
vector<Packet*> Packet::allPackets; 

Packet::Packet () 
  : target(0) 
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
  if (!target) {
    target = Army::getClosestFriendly(position, player1);
  }
 
  double distance = position.distance(target->position);
  if (speed*elapsedTime > distance) {
    target->supplies += size;
    return true; 
  }
  else {
    point dirVector = target->position;
    dirVector -= position;
    dirVector.normalise(); 
    dirVector *= speed*elapsedTime; 
    position += dirVector; 
  }
  
  return false; 
}
