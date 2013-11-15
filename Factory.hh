#ifndef FACTORY_HH
#define FACTORY_HH
#include <vector>
#include "utils.hh" 

using namespace std; 
class Packet;
class Tile; 
class Object;

struct WareHouse {
  WareHouse ();
  ~WareHouse ();

  bool release; 
  int capacity;
  int content; 
  bool player; 
  point position; 

  void receive (Packet* packet);
  void update ();

  typedef vector<WareHouse*>::iterator Iter;
  static Iter start () {return allWareHouses.begin();}
  static Iter final () {return allWareHouses.end();}

private:
  static vector<WareHouse*> allWareHouses;
};

struct Factory {
  friend void createFactory (Object*); 
  

  Factory ();
  ~Factory (); 
  bool player1; 
  int timeToProduce; // All times in microseconds
  int timeSinceProduction;
  int packetSize; 
  point position; 
  Tile* tile; 

  void produce (int elapsedTime);
  void toggle () {m_WareHouse.release = !m_WareHouse.release;} 

  typedef vector<Factory*>::iterator Iter;
  static Iter start () {return allFactories.begin();}
  static Iter final () {return allFactories.end();}

private:
  WareHouse m_WareHouse; 

  static vector<Factory*> allFactories;
};



#endif
