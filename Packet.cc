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
  return false; 
}
