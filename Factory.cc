#include "Factory.hh"
#include "Packet.hh" 

void Factory::produce (int elapsedTime, vector<Packet*>& packets) {
  timeSinceProduction += elapsedTime;
  if (timeSinceProduction < timeToProduce) return; 
  timeSinceProduction -= timeToProduce; 
  
  Packet* product = new Packet();
  product->size = packetSize;
  product->position = position;
  product->player1 = player1; 
  packets.push_back(product); 
}

