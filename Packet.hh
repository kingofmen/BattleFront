#ifndef PACKET_HH
#define PACKET_HH
#include <vector> 
#include "utils.hh" 

using namespace std; 
class Tile; 
class Vertex; 
class WareHouse; 

struct Packet {
  friend void initialise (); 

  Packet (); 
  ~Packet (); 
  bool player;
  int size;
  point position; 
  Tile* tile; 
  WareHouse* target; 

  int getSize () const {return size;} 
  bool update (int elapsedTime); 

  typedef vector<Packet*>::iterator Iter;
  static Iter start () {return allPackets.begin();}
  static Iter final () {return allPackets.end();} 

private:
  static vector<Packet*> allPackets; 
};


#endif 
