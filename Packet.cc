#include "Packet.hh"
#include "Army.hh" 

double Packet::speed = 0.0001; 

Packet::Packet () 
  : target(0) 
{}

bool Packet::update (int elapsedTime) {
  if (!target) {
    target = Army::getClosest(position);
  }
 
  double distance = position.distance(target->position);
  if (speed*elapsedTime > distance) {
    if (player1) target->supplies1 += size;
    else target->supplies2 += size; 
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
