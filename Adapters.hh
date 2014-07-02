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


#endif
