#ifndef UTILS_HH
#define UTILS_HH

#include "tuple/tuple.hpp"
#include <vector> 
#include <iostream> 

using namespace std;

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

 private:
  static vector<T*> allThings;
};

template <class T> vector<T*> Iterable<T>::allThings; 


#endif 
