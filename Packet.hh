#ifndef PACKET_HH
#define PACKET_HH

#include "utils.hh" 

class Army; 


struct Packet {
  Packet (); 
  bool player1;
  int size;
  point position; 
  Army* target; 

  bool update (int elapsedTime); 

  static double speed; // Pixels per microsecond
};


#endif 
