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
  static Button** increase;
  static Button** decrease;
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
  static Button** increaseUnits;
  static Button** decreaseUnits; 
  static WarehouseButtonAdapter* instance; 
};


#endif
