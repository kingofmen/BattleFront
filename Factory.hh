#ifndef FACTORY_HH
#define FACTORY_HH
#include <vector>
#include <list>
#include <deque>
#include "utils.hh"

using namespace std; 
class Tile; 
class Object;
struct WareHouse;

class RawMaterial : public Enumerable<RawMaterial> {
public:
  enum {Men = 0, Steel, Fuel, Ammo, NumRawMaterials};
  RawMaterial (string n, int i, bool f = false) : Enumerable<RawMaterial>(this, n, i, f) {}
private:
  static RawMaterial* RawMaterial1;
  static RawMaterial* RawMaterial2;
  static RawMaterial* RawMaterial3;
  static RawMaterial* RawMaterial4;
};

struct RawMaterialHolder {
  RawMaterialHolder ();
  RawMaterialHolder (double m, double s, double f, double a);
  ~RawMaterialHolder ();

  void add (RawMaterial const * const idx, double amt) {stockpile[*idx] += amt;}
  void clear ();
  double get (unsigned int idx) const {return stockpile[idx];}
  double get (RawMaterial const * const idx) const {return stockpile[*idx];}  
  double getMen   () const {return stockpile[RawMaterial::Men];}
  double getSteel () const {return stockpile[RawMaterial::Steel];}
  double getFuel  () const {return stockpile[RawMaterial::Fuel];}
  double getAmmo  () const {return stockpile[RawMaterial::Ammo];}
  double getWeight () const {return 1;}
  double getTotal () const; 
  void normalise  ();
  

  RawMaterialHolder& operator-= (const RawMaterialHolder& other);
  RawMaterialHolder& operator+= (const RawMaterialHolder& other);
  RawMaterialHolder& operator*= (const double scale);
  double& operator[] (unsigned int idx) {return stockpile[idx];}
  
private:
  vector<double> stockpile;
};

bool operator>= (const RawMaterialHolder& one, const RawMaterialHolder& two);
bool operator> (const RawMaterialHolder& one, const RawMaterialHolder& two);
RawMaterialHolder operator* (const RawMaterialHolder& rmh, double num);
RawMaterialHolder operator* (double num, const RawMaterialHolder& rmh);

class UnitType : public Enumerable<UnitType> {
public:
  enum {Regiment = 0, Train, Battery, Squadron, NumUnitTypes};
  UnitType (string n, string d, int i, bool f = false) : Enumerable<UnitType>(this, n, i, f), m_DisplayName(d) {} 
  string getDisplayName () const {return m_DisplayName;}
  
private:
  string m_DisplayName; 
  static UnitType* UnitType1;
  static UnitType* UnitType2;
  static UnitType* UnitType3;
  static UnitType* UnitType4;
};

struct UnitHolder {
  UnitHolder () : m_Units(UnitType::NumUnitTypes) {}
  ~UnitHolder () {}

  void clear (); 
  double getWeight () const {return 1;}
  int get (unsigned int idx) const            {return m_Units[idx];}
  int get (UnitType const* const idx) const   {return m_Units[*idx];}
  int& operator[] (unsigned int idx)          {return m_Units[idx];}
  int& operator[] (UnitType const* const idx) {return m_Units[*idx];}
  
private:
  vector<int> m_Units; 
};


struct Building {
  Building (point p);

  bool player; 
  point position; 
  Tile* tile; 

  virtual bool complete () const {return true;}
  bool checkOwnership (); 
  virtual double getCompFraction () const {return 1.0;}

protected:
  RawMaterialHolder m_Structure; // RMs used for the actual building, as opposed to stored in it. 
  void releaseTroops (int size, Tile* t = 0);
};

class CargoCar {
public:
  bool isRawMaterial () const {return m_CargoType != 0;}
  RawMaterial* getMaterial () const {return m_CargoType;}
  UnitType* getUnit () const {return m_UnitType;}
  void load (double amount) {m_CargoAmount += amount;}
  void load (int amount) {m_UnitAmount =+ amount;}
  void unloadInto (RawMaterialHolder* rmh);
  void unloadInto (UnitHolder* uh); 
  void unloadInto (RawMaterialHolder* rmh, UnitHolder* uh); 
  double getWeight () const {return m_CargoAmount;} 
  
  static CargoCar* getCargoCar (UnitType* unit);
  static CargoCar* getCargoCar (RawMaterial* rm); 
  static CargoCar* returnCar (CargoCar* car) {s_AvailableCars.push_back(car);}
    
private:
  CargoCar ();
  ~CargoCar ();

  static CargoCar* getCargoCar (); 
  static vector<CargoCar*> s_AvailableCars; 
  
  RawMaterial* m_CargoType;
  UnitType* m_UnitType;
  int m_UnitAmount;
  double m_CargoAmount; 
};

struct Locomotive : public Iterable<Locomotive>, public RawMaterialHolder {
  friend class StaticInitialiser;
  
  Locomotive (WareHouse* h);
  ~Locomotive (); 

  void load (CargoCar* car) {m_Cargo.push_back(car);}
  double getSpeedModifier () const {return maintenance;}
  void repair (int elapsedTime); 
  void traverse (double distance);
  double getLoadWeight () const {return m_Units.getWeight() + getWeight();}
  double getRepairState () const {return maintenance;} 

  typedef list<CargoCar*>::iterator CargoIter ;
  CargoIter startCargo () {return m_Cargo.begin();}
  CargoIter finalCargo () {return m_Cargo.end();}
  void clearCargo () {m_Cargo.clear();} 
  
  point position;
  Tile* tile; 
  double maintenance;
  WareHouse* home;
  WareHouse* destination; 
private:
  list<CargoCar*> m_Cargo; 
  UnitHolder m_Units; 
  
  static double decayRate;  // Inverse, negative pixels.
  static double repairRate; // Inverse, negative microseconds.
};

struct Railroad : public Building, public Iterable<Railroad> {
  friend class StaticInitialiser;
  
  Railroad (WareHouse* w1, WareHouse* w2); 

  void calcEnds ();
  virtual bool complete () const;
  virtual double getCompFraction () const; 
  int getLength () const; 
  bool receive (Locomotive* loco, WareHouse* source);
  void split (WareHouse* house);
  void update (int elapsedTime);
  double getNeededAmount (RawMaterial const* const rm) const;    // Returns the remaining needed
  double getStructureAmount (RawMaterial const* const rm) const; // Returns the total to build this railroad

  static Railroad* findConnector (WareHouse* w1, WareHouse* w2); 

  point oneEnd;
  point twoEnd;
  point ortho; 
  WareHouse* oneHouse;
  WareHouse* twoHouse; 

private:
  static double speed; // Pixels per microsecond
  static RawMaterialHolder s_Structure; // RMs per unit length. 
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

  void addRailroad (Railroad* r) {outgoing.push_back(r);}
  Railroad* connect (WareHouse* other);
  virtual bool complete () const;
  virtual double getCompFraction () const;
  double getNeededAmount (RawMaterial const* const rm) const;    // Returns the remaining needed
  double getStructureAmount (RawMaterial const* const rm) const; // Returns the total to build this warehouse
  void receive (Locomotive* loco, Railroad* source);
  void receive (UnitType* unit); 
  void replaceRail (Railroad* oldRail, Railroad* newRail);
  void sendLoco (WareHouse* other); 
  void toggleRail ();
  void update (int elapsedTime);
  
private:
  double getLoadCapacity () const {return 100;}
  Railroad* getOutgoingRailroad (CargoCar* cargo) const {return cargo->isRawMaterial() ? getOutgoingRailroad(cargo->getMaterial()) : getOutgoingRailroad(cargo->getUnit());}
  Railroad* getOutgoingRailroad (RawMaterial* rm) const;
  Railroad* getOutgoingRailroad (UnitType const* const rm) const;

  double m_LoadingCompletion;
  double m_UnloadingCompletion;
  map<Railroad*, list<CargoCar*> > m_Outgoing;
  list<CargoCar*> m_Unloading;
  CargoCar* m_Loading;
  map<void*, int> timeSinceLastLoad; 
  
  UnitHolder m_Units;
  UnitHolder m_UnitsDesired; 
  
  Railroad* activeRail; 
  vector<Railroad*> outgoing;
  WarehouseAI* m_ai;
  list<Locomotive*> locos;

  static RawMaterialHolder s_Structure;
  static UnitHolder s_DefaultUnitsDesired; 
};

class RawMaterialProducer : public Building, public Iterable<RawMaterialProducer> {
  friend class StaticInitialiser;
  friend class ProducerGraphics; 
public:
  RawMaterialProducer (WareHouse* w);
  ~RawMaterialProducer ();

  void produce (int elapsedTime); // In microseconds
  void increaseProduction (RawMaterial* rm);
  void decreaseProduction (RawMaterial* rm);
  
private:
  void setProduction (RawMaterial* rm, double prod);
  
  RawMaterialHolder maxProduction; // Units per microsecond
  RawMaterialHolder curProduction; // Fractions of maxProduction

  WareHouse* m_WareHouse; 
};

struct Factory : public Building, public Iterable<Factory> {
  friend class StaticInitialiser; 
  friend class FactoryGraphics; 
  
  Factory (point p);
  WareHouse m_WareHouse; 

  double getCompletion () const;   
  void orderLoco ();
  void orderUnit (UnitType* u); 
  void produce (int elapsedTime);
  
private:
  void doneProducing (); 
  void setCurrentProduction ();
  
  double m_Throughput; // Per microsecond
  RawMaterialHolder m_UsedSoFar;
  RawMaterialHolder m_NormalisedCost; 
  int unableToProgress; 
  deque<UnitType*> m_ProductionQueue; 
  static vector<RawMaterialHolder> s_ProductionCosts; // Costs to make one unit.
};



#endif
