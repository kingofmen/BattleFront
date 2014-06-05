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
  Graphics<Drawer, Target> (Drawer* d, Target* t, point p) : Iterable<Drawer>(d), m_Drawable(t), location(p) {}
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

  static Target* select (point p) {
    double bestDist = 1e20;
    for (typename Iterable<Drawer>::Iter i = Iterable<Drawer>::start(); i != Iterable<Drawer>::final(); ++i) {
      double currDist = (*i)->location.distanceSq(p);
      if (currDist > 100) continue; 
      if (currDist > bestDist) continue;
      bestDist = currDist;
      selected = (*i);
    }
    return selected;
  }

  static Target* getSelected () {return selected;}
  static void unSelect () {selected = 0;}
  
protected:
  Target* m_Drawable;
  point location;
  static Target* selected; 
};

template<typename Drawer, typename Target> Target* Graphics<Drawer, Target>::selected = 0; 

class WareHouseGraphics : public Graphics<WareHouseGraphics, WareHouse> {
public:
  WareHouseGraphics (WareHouse* w, point p);
  ~WareHouseGraphics ();

  void draw () const;

private:

}; 

class ProducerGraphics : public Graphics<ProducerGraphics, RawMaterialProducer> {
public:
  ProducerGraphics (RawMaterialProducer* rmp, point p);
  ~ProducerGraphics ();

  void draw () const;

private:

};

class FactoryGraphics : public Graphics<FactoryGraphics, Factory> {
  friend class StaticInitialiser;
public:
  FactoryGraphics (Factory* f, point p);
  ~FactoryGraphics (); 

  void draw () const;
  
private:

};


#endif 
