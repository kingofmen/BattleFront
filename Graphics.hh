#ifndef GRAPHICS_HH
#define GRAPHICS_HH

#ifdef WINDOWS
#include <gl/gl.h>
#else
#include "gl.h"
#endif

#include "utils.hh"
#include "Factory.hh"

template <typename Drawer, typename Target> class Graphics : public Iterable<Drawer> {
public:
  Graphics<Drawer, Target> (Drawer* d, Target* t) : Iterable<Drawer>(d), m_Drawable(t) {}
  ~Graphics<Drawer, Target> () {}

  virtual void draw () const = 0; 
  static Drawer* findGraphicsObject (Target const* const t) {
    for (typename Iterable<Drawer>::Iter i = Iterable<Drawer>::start();
	 i != Iterable<Drawer>::final();
	 ++i) {
      if ((*i)->m_Drawable == t) return (*i);
    }
    return 0; 
  }

  
protected:
  Target* m_Drawable; 
};

class WareHouseGraphics : public Graphics<WareHouseGraphics, WareHouse> {
public:
  WareHouseGraphics (WareHouse* w);
  ~WareHouseGraphics ();

  void draw () const;

private:

}; 

class ProducerGraphics : public Graphics<ProducerGraphics, RawMaterialProducer> {
public:
  ProducerGraphics (RawMaterialProducer* p);
  ~ProducerGraphics ();

  void draw () const;

private:

};

class FactoryGraphics : public Graphics<FactoryGraphics, Factory> {
  friend class StaticInitialiser;
public:
  FactoryGraphics (Factory* f);
  ~FactoryGraphics (); 

  void draw () const;
  
private:

  WareHouseGraphics* m_WareHouseDrawer;
  ProducerGraphics* m_ProducerDrawer; 
};


#endif 
