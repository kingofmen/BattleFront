#ifndef UTILS_HH
#define UTILS_HH

#include "tuple/tuple.hpp"
#include <vector> 
#include <iostream>
#include <sys/time.h>
#include "SDL.h"
#include "SDL_ttf.h"

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

 private:
  static vector<T*> allThings;
};

template <class T> vector<T*> Iterable<T>::allThings; 

// Texture wrapper class, blatantly stolen from LazyFoo tutorial
// but somewhat modified to target OpenGL rendering. 
class LTexture {
public:
  // Initializes variables
  LTexture();
  // Deallocates memory
  ~LTexture();

  //Loads image at specified path
  bool loadFromFile (string path);
		
  // Creates image from font string
  bool loadFromRenderedText (string textureText, SDL_Color textColor, TTF_Font* gFont);

  // Deallocates texture
  void free();
 
  // Renders texture at given point
  void render (int x, int y); 

  // Set color modulation
  void setColor(Uint8 red, Uint8 green, Uint8 blue);
  // Set blending
  void setBlendMode (SDL_BlendMode blending);
  // Set alpha modulation
  void setAlpha(Uint8 alpha);
  
  // Gets image dimensions
  int getWidth();
  int getHeight();

private:
  GLuint mTexture; 
  
  // Image dimensions
  int mWidth;
  int mHeight;
};


#endif 
