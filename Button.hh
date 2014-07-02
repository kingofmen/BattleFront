#ifndef BUTTON_HH
#define BUTTON_HH

#include "utils.hh"
#include <vector>

class Button : public Clickable<Button>, public Numbered<Button> {
  friend class ButtonGraphics; 
public:
  Button (int x, int y, int width, int height);
  ~Button ();

  void click ();
  bool isActive () const {return active;} 
  void registerListener (Listener* listener) {m_listeners.push_back(listener);}
  void setActive (bool a = true) {active = a;}
  
private:
  bool active; 
  vector<Listener*> m_listeners;
};


#endif
