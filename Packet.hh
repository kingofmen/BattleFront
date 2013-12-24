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

  enum Product {Manpower = 0, Gasoline, Materiel, NumProducts};

  Packet ();
  Packet (int m, int g, int a); 
  ~Packet (); 
  
  void add (Product p, int amount) {content[p] += amount;}
  void clear ();
  int getSize () const {return accumulate(content.begin(), content.end(), 0);}
  int getManpower () const {return content[Manpower];}
  int getGasoline () const {return content[Gasoline];}
  int getMateriel () const {return content[Materiel];}
  int getAmount (Product p) const {return content[p];}
  Packet& operator+= (const Packet& other) {transform(content.begin(), content.end(), other.content.begin(), content.begin(), plus<int>()); return *this;}
  Packet& operator-= (const Packet& other) {transform(content.begin(), content.end(), other.content.begin(), content.begin(), minus<int>()); return *this;}
  
  
  bool player;
  point position; 
  Tile* tile; 

private:
  vector<int> content; 
};

bool operator>  (const Packet& one, const Packet& two);
bool operator>= (const Packet& one, const Packet& two);
bool operator<  (const Packet& one, const Packet& two);
bool operator<= (const Packet& one, const Packet& two);
ostream& operator<< (ostream& o, const Packet& p); 

#endif 
