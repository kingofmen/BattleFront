#ifndef FACTORY_HH
#define FACTORY_HH
#include <vector>
#include "utils.hh" 

using namespace std; 
class Packet;
class Tile; 
class Object;
struct WareHouse;

struct Building {
  Building ();

  bool player; 
  point position; 
  int capacity;
  int toCompletion; 
};

struct Railroad : public Building, public Iterable<Railroad> {
  Railroad (WareHouse* w1, WareHouse* w2); 

  bool canAccept (Packet* packet);
  void receive (Packet* packet);

  point oneEnd;
  point twoEnd;
  WareHouse* oneHouse;
  WareHouse* twoHouse; 
  int currentLoad; 

};

struct WareHouse : public Building, public Iterable<WareHouse> {
  WareHouse ();

  bool release; 
  int content; 

  void receive (Packet* packet);
  void update ();
  void addRailroad (Railroad* r) {outgoing.push_back(r);}

private:
  Railroad* activeRail; 
  vector<Railroad*> outgoing; 
};

struct Factory : public Building, public Iterable<Factory> {
  friend void createFactory (Object*); 

  Factory ();
  int timeToProduce; // All times in microseconds
  int timeSinceProduction;
  Tile* tile; 

  void produce (int elapsedTime);
  void toggle () {m_WareHouse.release = !m_WareHouse.release;} 

  //typedef vector<Factory*>::iterator Iter;
  //static Iter start () {return allFactories.begin();}
  //static Iter final () {return allFactories.end();}

private:
  WareHouse m_WareHouse; 

  //static vector<Factory*> allFactories;
};



#endif
