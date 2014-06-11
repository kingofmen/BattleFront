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

  static Target* getClicked (point p, Target** closest = 0) {
    double bestDist = 1e20;
    Target* clicked = 0;
    for (typename Iterable<Drawer>::Iter i = Iterable<Drawer>::start(); i != Iterable<Drawer>::final(); ++i) {
      if (!(*i)->m_Drawable->player) continue; 
      double currDist = (*i)->m_Drawable->position.distanceSq(p);
      if (currDist > bestDist) continue;
      bestDist = currDist;       
      if (closest) closest[0] = (*i)->m_Drawable;
      if (currDist > 100) continue; 
      clicked = (*i)->m_Drawable;
    }
    return clicked; 
  }

  static void select (Target* s) {selected = s;}
  static Target* getSelected () {return selected;}
  static void unSelect () {selected = 0;}
  
protected:
  Target* m_Drawable;
  static Target* selected; 
};

template<typename Drawer, typename Target> Target* Graphics<Drawer, Target>::selected = 0; 

class WareHouseGraphics : public Graphics<WareHouseGraphics, WareHouse> {
public:
  WareHouseGraphics (WareHouse* w);
  ~WareHouseGraphics ();

  void draw () const;

private:

}; 

class ProducerGraphics : public Graphics<ProducerGraphics, RawMaterialProducer> {
public:
  ProducerGraphics (RawMaterialProducer* rmp);
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

};


#endif 
