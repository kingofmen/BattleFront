#ifndef FACTORY_HH
#define FACTORY_HH
#include <vector>
#include <list>
#include <queue>
#include "utils.hh"
#include "Packet.hh" 

using namespace std; 
class Tile; 
class Object;
struct WareHouse;

enum RawMaterials {Men = 0, Steel, Fuel, Ammo, NumRawMaterials};
enum UnitType {Regiment = 0, Train, Battery, Squadron, NumUnitTypes}; 

struct RawMaterialHolder {
  RawMaterialHolder ();
  RawMaterialHolder (double m, double s, double f, double a);
  ~RawMaterialHolder ();

  void add (unsigned int idx, double amt) {stockpile[idx] += amt;}
  void clear ();
  double get (unsigned int idx) const {return stockpile[idx];}
  double getMen   () const {return stockpile[Men];}
  double getSteel () const {return stockpile[Steel];}
  double getFuel  () const {return stockpile[Fuel];}
  double getAmmo  () const {return stockpile[Ammo];}
  void normalise  (); 

  RawMaterialHolder& operator-= (const RawMaterialHolder& other) {for (int i = 0; i < NumRawMaterials; ++i) stockpile[i] -= other.stockpile[i]; return *this;}
  RawMaterialHolder& operator+= (const RawMaterialHolder& other) {for (int i = 0; i < NumRawMaterials; ++i) stockpile[i] += other.stockpile[i]; return *this;}
  RawMaterialHolder& operator*= (const double scale) {for (int i = 0; i < NumRawMaterials; ++i) stockpile[i] *= scale; return *this;}
  
  static string getName (RawMaterials r);
  static string getName (unsigned int r); 
  
private:
  vector<double> stockpile; 
};

bool operator>= (const RawMaterialHolder& one, const RawMaterialHolder& two);
RawMaterialHolder operator* (const RawMaterialHolder& rmh, double num);
RawMaterialHolder operator* (double num, const RawMaterialHolder& rmh);

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

struct WareHouse : public Building, public RawMaterialHolder, public Iterable<WareHouse> {
  friend class StaticInitialiser;
  friend class WareHouseGraphics; 
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
  //void add (unsigned int idx, double amount) {add(idx, amount);}
  
private:
  static int newBuildSize; 
  Railroad* activeRail; 
  vector<Railroad*> outgoing;
  WarehouseAI* m_ai;
  list<Locomotive*> locos;
};

class RawMaterialProducer : public Building, public Iterable<RawMaterialProducer> {
  friend class StaticInitialiser; 
public:
  RawMaterialProducer (WareHouse* w);
  ~RawMaterialProducer ();

  void produce (int elapsedTime); // In microseconds
  
private:
  RawMaterialHolder maxProduction; // Units per microsecond
  RawMaterialHolder curProduction; // Fractions of maxProduction

  WareHouse* m_WareHouse; 
};

struct Factory : public Building, public Iterable<Factory> {
  friend class StaticInitialiser; 
  friend class FactoryGraphics; 
  
  Factory (point p);
  WareHouse m_WareHouse; 

  void orderLoco (); 
  void produce (int elapsedTime);

  static string getName (UnitType ut);
  static string getName (unsigned int ut); 

private:
  void doneProducing (); 
  void setCurrentProduction ();
  
  double m_Throughput; // Per microsecond
  RawMaterialHolder m_UsedSoFar;
  RawMaterialHolder m_NormalisedCost; 
  int unableToProgress; 
  queue<UnitType> m_ProductionQueue; 
  static vector<RawMaterialHolder> s_ProductionCosts; // Costs to make one unit.
};



#endif
