#ifndef PACKET_HH
#define PACKET_HH
#include <vector> 
#include "utils.hh" 

using namespace std; 
class Tile; 
class Vertex; 
class WareHouse; 

struct Packet : public Supplies {
  friend void initialise (); 

  Packet (); 
  ~Packet (); 
  bool player;
  point position; 
  Tile* tile; 
  WareHouse* target; 

private:
  static vector<Packet*> allPackets; 
};


#endif 
