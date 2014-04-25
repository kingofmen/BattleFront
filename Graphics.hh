#ifndef GRAPHICS_HH
#define GRAPHICS_HH

#ifdef WINDOWS
#include <gl/gl.h>
#else
#include "gl.h"
#endif

#include "Factory.hh"

class WareHouseGraphics : public Iterable<WareHouseGraphics> {
public:
  WareHouseGraphics (WareHouse* w);
  ~WareHouseGraphics ();

  void draw ();

private:
  WareHouse* m_WareHouse; 
}; 

class FactoryGraphics : public Iterable<FactoryGraphics> {
public:
  FactoryGraphics (Factory* f);
  ~FactoryGraphics (); 

  void draw ();
  
private:
  Factory* m_Factory; 
  WareHouseGraphics* m_WareHouseDrawer;
};


#endif 
