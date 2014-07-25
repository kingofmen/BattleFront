#ifndef ADAPTERS_HH
#define ADAPTERS_HH

#include "utils.hh"
class Button; 

class ProducerButtonAdapter : public Listener {
public:
  static ProducerButtonAdapter* getInstance (); 
  void clicked (unsigned int buttonId);
  void setActive (bool act = true);
  
private:
  ProducerButtonAdapter () {}
  static Button** s_Increase;
  static Button** s_Decrease;
  static ProducerButtonAdapter* instance; 
};

class FactoryButtonAdapter : public Listener {
public:
  static FactoryButtonAdapter* getInstance (); 
  void clicked (unsigned int buttonId);
  void setActive (bool act = true);
  
private:
  FactoryButtonAdapter () {}
  static Button** orderUnits; 
  static FactoryButtonAdapter* instance; 
};

class WarehouseButtonAdapter : public Listener {
public:
  static WarehouseButtonAdapter* getInstance (); 
  void clicked (unsigned int buttonId);
  void setActive (bool act = true);
  
private:
  WarehouseButtonAdapter () {}
  static Button** s_IncreaseUnits;
  static Button** s_DecreaseUnits;
  static Button* s_IncArtillery;
  static Button* s_DecArtillery;
  static Button* s_IncAircraft;
  static Button* s_DecAircraft;
  static WarehouseButtonAdapter* instance; 
};


#endif
