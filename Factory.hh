#ifndef FACTORY_HH
#define FACTORY_HH
#include <vector>
#include "utils.hh" 

using namespace std; 
class Packet;
class Tile; 
class Object;

struct Building {
  Building ();
  ~Building ();

  bool player; 
  point position; 
  int capacity;
  int toCompletion; 
};

struct Railroad : public Building {
  Railroad (); 

  bool canAccept (Packet* packet);
  void receive (Packet* packet);

  point oneEnd;
  point twoEnd;
  int currentLoad; 
  
};

struct WareHouse : public Building {
  WareHouse ();
  ~WareHouse ();

  bool release; 
  int content; 

  void receive (Packet* packet);
  void update ();

  typedef vector<WareHouse*>::iterator Iter;
  static Iter start () {return allWareHouses.begin();}
  static Iter final () {return allWareHouses.end();}

private:
  Railroad* activeRail; 
  vector<Railroad*> outgoing; 
  static vector<WareHouse*> allWareHouses;
};

struct Factory : public Building {
  friend void createFactory (Object*); 

  Factory ();
  ~Factory (); 
  int timeToProduce; // All times in microseconds
  int timeSinceProduction;
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
