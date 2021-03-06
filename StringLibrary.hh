#ifndef STRING_LIBRARY_HH
#define STRING_LIBRARY_HH

#include <string>
#include <map>
#include "SDL.h"
#include "SDL_ttf.h"
#include "utils.hh"

using namespace std;

// Texture wrapper class, blatantly stolen from LazyFoo tutorial
// but somewhat modified to target OpenGL rendering. 
class LTexture {
public:
  // Initializes variables
  LTexture();
  // Deallocates memory
  ~LTexture();

  // Loads image at specified path
  bool loadFromFile (string path);
		
  // Creates image from font string
  bool loadFromRenderedText (string textureText, SDL_Color textColor, TTF_Font* gFont);

  // Deallocates texture
  void free();
 
  // Renders texture at given point
  void render (double x, double y); 

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

class StringLibrary {
public:
  StringLibrary (SDL_Color tc, TTF_Font* tf);

  unsigned int getWidth (unsigned int idx) const {return library[idx]->getWidth();} 
  unsigned int renderFloat (double txt, int precision, double x, double y); 
  unsigned int renderInt (int txt, double x, double y);
  unsigned int registerText (string txt);  
  unsigned int renderText (string txt, double x, double y);
  void         renderText (unsigned int id, double x, double y);
  
private:
  SDL_Color textColour;
  TTF_Font* textFont; 
  map<string, unsigned int> index; 
  vector<LTexture*> library;
};

#endif
