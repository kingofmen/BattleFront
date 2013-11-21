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
  Building (point p);

  bool player; 
  point position; 
  int capacity;
  int toCompletion; 
  Tile* tile; 

  double getCompFraction () {return 1.0 - 0.001*toCompletion;} 

protected:
  bool useToBuild (Packet* packet); 
};

struct Railroad : public Building, public Iterable<Railroad> {
  Railroad (WareHouse* w1, WareHouse* w2); 

  bool canAccept (Packet* packet);
  void receive (Packet* packet, WareHouse* source);
  void update (int elapsedTime);

  point oneEnd;
  point twoEnd;
  WareHouse* oneHouse;
  WareHouse* twoHouse; 
  int currentLoad; 

private:
  vector<Packet*> packets; 
};

struct WareHouse : public Building, public Iterable<WareHouse> {
  WareHouse (point p);

  bool release; 
  int content; 

  void addRailroad (Railroad* r) {outgoing.push_back(r);}
  void receive (Packet* packet);
  void releaseTroops (int size); 
  void toggle (); 
  void update ();
  

private:
  Railroad* activeRail; 
  vector<Railroad*> outgoing; 
};

struct Factory : public Building, public Iterable<Factory> {
  friend void createFactory (Object*); 

  Factory (point p);
  int timeToProduce; // All times in microseconds
  int timeSinceProduction;
  WareHouse m_WareHouse; 

  void produce (int elapsedTime);
  void toggle () {m_WareHouse.release = !m_WareHouse.release;} 
  


private:
};



#endif
