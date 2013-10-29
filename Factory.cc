#include "Factory.hh"
#include "Packet.hh" 

void Factory::produce (int elapsedTime) {
  timeSinceProduction += elapsedTime;
  if (timeSinceProduction < timeToProduce) return; 
  timeSinceProduction -= timeToProduce; 
  
  Packet* product = new Packet();
  product->size = packetSize;
  product->position = position;
  product->player1 = player1; 
}

