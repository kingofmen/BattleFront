#ifndef PACKET_HH
#define PACKET_HH
#include <vector> 
#include "utils.hh" 

using namespace std; 
class Army; 


struct Packet {
  Packet (); 
  ~Packet (); 
  bool player1;
  int size;
  point position; 
  Army* target; 

  bool update (int elapsedTime); 

  typedef vector<Packet*>::iterator Iter;
  static Iter start () {return allPackets.begin();}
  static Iter final () {return allPackets.end();} 

  static double speed; // Pixels per microsecond

private:
  static vector<Packet*> allPackets; 
};


#endif 
