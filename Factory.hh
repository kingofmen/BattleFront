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

  virtual double getCompFraction () const {return 1.0 - 0.001*toCompletion;} 

protected:
  bool useToBuild (Packet* packet); 
  void releaseTroops (int size, Tile* t = 0);
};

struct Railroad : public Building, public Iterable<Railroad> {
  friend void initialise ();
  
  Railroad (WareHouse* w1, WareHouse* w2); 

  void calcEnds (); 
  bool canAccept (Packet* packet);
  virtual double getCompFraction () const; 
  int getLength () const; 
  void receive (Packet* packet, WareHouse* source);
  void update (int elapsedTime);
  void upgrade (); 

  static Railroad* findConnector (WareHouse* w1, WareHouse* w2); 

  point oneEnd;
  point twoEnd;
  point ortho; 
  WareHouse* oneHouse;
  WareHouse* twoHouse; 
  int currentLoad; 
  int locosToBuild; 

private:
  static double speed; // Pixels per microsecond
  vector<Packet*> packets; 
};

struct WareHouse : public Building, public Iterable<WareHouse> {
  friend void drawRailroads(); 

  WareHouse (point p);

  bool release; 
  int content; 

  void addRailroad (Railroad* r) {outgoing.push_back(r);}
  void receive (Packet* packet);
  void replaceRail (Railroad* oldRail, Railroad* newRail);
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

private:
};



#endif
