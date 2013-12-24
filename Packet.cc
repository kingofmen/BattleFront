#include "Packet.hh"
#include "Tile.hh" 
#include <cmath> 

Packet::Packet () 
  : tile(0)
  , content(NumProducts)
{}

Packet::Packet (int m, int g, int a) 
  : tile(0)
  , content(NumProducts)
{
  content[Manpower] = m;
  content[Gasoline] = g;
  content[Materiel] = a; 
}

Packet::~Packet () {}

void Packet::clear () {for (int i = Manpower; i < NumProducts; ++i) {content[i] = 0;}}

bool operator> (const Packet& one, const Packet& two) {
  if (one.getManpower() <= two.getManpower()) return false;
  if (one.getGasoline() <= two.getGasoline()) return false;
  if (one.getMateriel() <= two.getMateriel()) return false;
  return true;
}
bool operator< (const Packet& one, const Packet& two) {
  if (one.getManpower() < two.getManpower()) return true;
  if (one.getGasoline() < two.getGasoline()) return true;
  if (one.getMateriel() < two.getMateriel()) return true;
  return false;
}
bool operator>= (const Packet& one, const Packet& two) {
  if (one.getManpower() < two.getManpower()) return false;
  if (one.getGasoline() < two.getGasoline()) return false;
  if (one.getMateriel() < two.getMateriel()) return false;
  return true;
}
bool operator<= (const Packet& one, const Packet& two) {
  return !(one > two); 
}

ostream& operator<< (ostream& os, const Packet& p) {
  os << "(" << p.getManpower() << ", " << p.getGasoline() << " " << p.getMateriel() << ")";
  return os;
}
