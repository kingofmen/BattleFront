#ifndef UTILS_HH
#define UTILS_HH

#include "tuple/tuple.hpp"
#include <vector> 
#include <iostream>
#include <sys/time.h>
#include <algorithm>
#include <functional> 
#include <numeric> 
#include <cassert>
#include <map> 

using namespace std;

#ifdef WINDOWS
#include <gl/gl.h>
inline void timersub (const timeval* tvp, const timeval* uvp, timeval* vvp) {
  vvp->tv_sec = tvp->tv_sec - uvp->tv_sec;
  vvp->tv_usec = tvp->tv_usec - uvp->tv_usec;
  if (vvp->tv_usec < 0) {
     --vvp->tv_sec;
     vvp->tv_usec += 1000000;
  }
}
#else
#include "gl.h" 
#endif 


struct point : public boost::tuple<double, double> {
  point ();
  point (double x, double y);

  double& x();
  double& y();
  double const& x() const;
  double const& y() const;

  double angle (const point& other) const; 
  double distance (const point& other) const;
  double distanceSq (const point& other) const;
  double length () const;
  double lengthSq () const;
  void normalise ();
  void rotate (double angle); // Clockwise rotation
  point& operator+= (const point& other);
  point& operator-= (const point& other);
  point& operator*= (const double scale);
  point& operator/= (const double scale);
};

inline point operator+ (point one, const point& two) {
  one += two;
  return one; 
}

inline point operator* (point one, const double two) {
  one *= two;
  return one; 
}

inline point operator/ (point one, const double two) {
  one /= two;
  return one; 
}

inline point operator- (point one, const point& two) {
  one -= two;
  return one; 
}

ostream& operator<< (ostream& os, const point& p); 

template <class T> class Iterable {
 public:

  Iterable<T> (T* dat) {allThings.push_back(dat);} 
  ~Iterable<T> () {
    for (unsigned int i = 0; i < allThings.size(); ++i) {
      if (allThings[i] != this) continue;
      allThings[i] = allThings.back();
      break;
    }
    allThings.pop_back(); 
  }

  typedef typename vector<T*>::iterator Iter;
  static Iter start () {return allThings.begin();}
  static Iter final () {return allThings.end();}
  static unsigned int totalAmount () {return allThings.size();}
  
 private:
  static vector<T*> allThings;
};

template <class T> vector<T*> Iterable<T>::allThings; 

template <class T> class Finalizable {
public:
  Finalizable<T> (bool final = false) {
    assert(!s_Final);
    s_Final = final;
  }

private:
  static bool s_Final;
};

template<class T> bool Finalizable<T>::s_Final = false; 

template<class T> class Named {
public:
  Named (string n, T* dat) : name(n) {assert(!nameToObjectMap[n]); nameToObjectMap[n] = dat;}
  string getName () const {return name;}
  static T* getByName (string n) {return nameToObjectMap[n];}
private:
  string name;
  static map<string, T*> nameToObjectMap;
};

template<class T> map<string, T*> Named<T>::nameToObjectMap;

template<class T> class Numbered {
public:
  Numbered<T> (T* dat, unsigned int i) : idx(i) {
    if (i >= theNumbers.size()) theNumbers.resize(i+1);
    theNumbers[i] = dat;
  }

  Numbered<T> (T* dat) : idx(theNumbers.size()) {
    theNumbers.push_back(dat); 
  }

  unsigned int getIdx () const {return idx;}
  static T* getByIndex (unsigned int i) {return theNumbers[i];}
  operator unsigned int() const {return idx;}
  
private:
  unsigned int idx; 
  static vector<T*> theNumbers;
};

template<class T> vector<T*> Numbered<T>::theNumbers; 

template<class T> class Enumerable : public Finalizable<T>, public Iterable<T>, public Named<T>, public Numbered<T> {
public:
  Enumerable<T> (T* dat, string n, int i, bool final = false) 
    : Finalizable<T>(final)
    , Iterable<T>(dat)
    , Named<T>(n, dat)
    , Numbered<T>(dat, i)
  {}
  
private:
};

template<class T> class Clickable : public Iterable<T> {
public:
  Clickable<T> (T* dat, int x, int y, int w, int h)
    : Iterable<T>(dat)
    , xpos(x)
    , ypos(y)
    , width(w)
    , height(h) {}

  static T* getClicked (point p) {
    return getClicked(p.x(), p.y());
  }
  
  static T* getClicked (int x, int y) {
    for (typename T::Iter i = T::start(); i != T::final(); ++i) {
      if ((*i)->isClicked(x, y)) return (*i);
    }
    return 0;
  }

  static T* getClicked (double x, double y) {
    for (typename T::Iter i = T::start(); i != T::final(); ++i) {
      if ((*i)->isClicked(x, y)) return (*i);
    }
    return 0;
  }
 
  bool isClicked (int x, int y) {
    if (x < xpos) return false;
    if (x > xpos + width) return false;
    if (y < ypos) return false;
    if (y > ypos + height) return false;
    return true;
  }

  bool isClicked (double x, double y) {
    if (x < xpos) return false;
    if (x > xpos + width) return false;
    if (y < ypos) return false;
    if (y > ypos + height) return false;
    return true;
  }
  

protected:
  int xpos;
  int ypos;
  int width;
  int height;
};

class Listener {
public:
  virtual ~Listener ();
  virtual void clicked (unsigned int buttonId) = 0;
};



#endif 
