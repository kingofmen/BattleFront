#ifndef FACTORY_HH
#define FACTORY_HH
#include <vector>
#include "utils.hh" 

using namespace std; 
class Packet;
class Tile; 

struct Factory {
  bool player1; 
  int timeToProduce; // All times in microseconds
  int timeSinceProduction;
  int packetSize; 
  point position; 
  Tile* tile; 

  void produce (int elapsedTime);
};



#endif
