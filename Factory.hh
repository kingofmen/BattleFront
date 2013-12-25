#ifndef FACTORY_HH
#define FACTORY_HH
#include <vector>
#include <list>
#include "utils.hh"
#include "Packet.hh" 

using namespace std; 
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

  bool complete () const {return (0 == toCompletion);} 
  virtual double getCompFraction () const {return 1.0 - 0.001*toCompletion;} 

protected:
  bool useToBuild (Packet* packet); 
  void releaseTroops (int size, Tile* t = 0);
};

struct Locomotive : public Iterable<Locomotive> {
  friend class StaticInitialiser;
  
  Locomotive (WareHouse* h);
  ~Locomotive (); 

  double getSpeedModifier () const {return maintenance;}
  void repair (int elapsedTime); 
  void traverse (double distance); 
  
  point position;
  Tile* tile; 
  double maintenance;
  WareHouse* home;
  WareHouse* destination; 
  Packet* load;
private:
  static double decayRate;  // Inverse, negative pixels.
  static double repairRate; // Inverse, negative microseconds.
};

struct Railroad : public Building, public Iterable<Railroad> {
  friend class StaticInitialiser;
  
  Railroad (WareHouse* w1, WareHouse* w2); 

  void calcEnds (); 
  virtual double getCompFraction () const; 
  int getLength () const; 
  void receive (Locomotive* loco, WareHouse* source);
  void update (int elapsedTime);

  static Railroad* findConnector (WareHouse* w1, WareHouse* w2); 

  point oneEnd;
  point twoEnd;
  point ortho; 
  WareHouse* oneHouse;
  WareHouse* twoHouse; 

private:
  static double speed; // Pixels per microsecond
  list<Locomotive*> locos; 
};

class WarehouseAI {
  friend void drawFactories ();
  friend class StaticInitialiser;  
public:
  WarehouseAI (WareHouse* w);
  enum Action {Passed, Released, Held};
  
  static void globalAI (); 
  void notify (int size, Action act);
  void setReinforceTarget (WareHouse* t); 
  void update (int elapsedTime); 
  
  
private:
  WareHouse* m_WareHouse;
  WareHouse* reinforceTarget;
  double reinforcePercentage;
  bool statusChanged; 
  Railroad* connection;
  list<pair<int, Action> > packets;
  int threatLevel;

  static int defcon5;
  static int defcon4;
  static int defcon3;
  static int defcon2;  
};

struct WareHouse : public Building, public Iterable<WareHouse> {
  friend class StaticInitialiser;
  friend void drawRailroads();
  friend void drawFactories ();  
  friend class WarehouseAI; 
  
  WareHouse (point p);

  enum State {Accumulate, Hold, Release}; 
  State release; 
  int content; 

  void addRailroad (Railroad* r) {outgoing.push_back(r);}
  Railroad* connect (WareHouse* other); 
  virtual double getCompFraction () const; 
  void receive (Packet* packet);
  void receive (Locomotive* loco, Railroad* source); 
  void replaceRail (Railroad* oldRail, Railroad* newRail);
  void sendLoco (WareHouse* other); 
  void toggleHoldState (bool backwards);   
  void toggleRail ();
  void update (int elapsedTime);
  

private:
  static int newBuildSize; 
  Railroad* activeRail; 
  vector<Railroad*> outgoing;
  WarehouseAI* m_ai;
  list<Locomotive*> locos; 
};

struct Factory : public Building, public Iterable<Factory> {
  friend class StaticInitialiser; 

  Factory (point p);
  int timeToProduce; // All times in microseconds
  int timeSinceProduction;
  WareHouse m_WareHouse; 

  void orderLoco (); 
  void produce (int elapsedTime);

private:
  Packet production;
  Packet produced; 
  int queuedLocos; 

  static Packet locomotiveCost;
  static Packet regimentalCost;
};



#endif
