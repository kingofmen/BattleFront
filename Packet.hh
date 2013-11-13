#ifndef PACKET_HH
#define PACKET_HH
#include <vector> 
#include "utils.hh" 

using namespace std; 
class Tile; 
class Vertex; 

struct Packet {
  friend void initialise (); 

  Packet (); 
  ~Packet (); 
  bool player1;
  int size;
  point position; 
  Tile* tile; 
  Vertex* target; 

  bool update (int elapsedTime); 

  typedef vector<Packet*>::iterator Iter;
  static Iter start () {return allPackets.begin();}
  static Iter final () {return allPackets.end();} 

private:
  static double speed; // Pixels per microsecond
  static vector<Packet*> allPackets; 
};


#endif 
