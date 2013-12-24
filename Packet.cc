#include "Packet.hh"
#include "Tile.hh" 
#include <cmath> 

Packet::Packet () 
  : tile(0)
  , content(NumProducts)
{}

Packet::~Packet () {}

void Packet::clear () {for (int i = Manpower; i < NumProducts; ++i) {content[i] = 0;}}

bool operator> (const Packet& one, const Packet& two) {
  if (one.getManpower() <= two.getManpower()) return false;
  if (one.getGasoline() <= two.getGasoline()) return false;
  if (one.getMateriel() <= two.getMateriel()) return false;
  return true;
}
bool operator< (const Packet& one, const Packet& two) {
  if (one.getManpower() >= two.getManpower()) return false;
  if (one.getGasoline() >= two.getGasoline()) return false;
  if (one.getMateriel() >= two.getMateriel()) return false;
  return true;
}
bool operator>= (const Packet& one, const Packet& two) {
  if (one.getManpower() < two.getManpower()) return false;
  if (one.getGasoline() < two.getGasoline()) return false;
  if (one.getMateriel() < two.getMateriel()) return false;
  return true;
}
bool operator<= (const Packet& one, const Packet& two) {
  if (one.getManpower() > two.getManpower()) return false;
  if (one.getGasoline() > two.getGasoline()) return false;
  if (one.getMateriel() > two.getMateriel()) return false;
  return true;
}


