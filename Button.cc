#include "Button.hh"

Button::Button (int x, int y, int w, int h)
  : Clickable<Button>(this, x, y, w, h)
  , Numbered<Button>(this)
  , active(false)
{}

Button::~Button () {}

void Button::click () {
  if (!active) return; 
  for (vector<Listener*>::iterator l = m_listeners.begin(); l != m_listeners.end(); ++l) {
    (*l)->clicked(getIdx()); 
  }
}
