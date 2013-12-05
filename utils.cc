#include "utils.hh" 
#include <cmath> 

point::point () 
  : boost::tuple<double, double>(0, 0) 
{}

point::point (double x, double y)  
  : boost::tuple<double, double>(x, y) 
{}

double& point::x() {return boost::get<0>(*this);}
double& point::y() {return boost::get<1>(*this);}

double const& point::x() const {return boost::get<0>(*this);}
double const& point::y() const {return boost::get<1>(*this);}

double point::angle (const point& other) const {
  double dot = x() * other.x();
  dot       += y() * other.y(); 
  dot       /= length();
  dot       /= other.length(); 
  return acos(dot); 
}

double point::distanceSq (const point& other) const {
  return pow(x() - other.x(), 2) + pow(y() - other.y(), 2); 
}

double point::distance (const point& other) const {
  return sqrt(distanceSq(other)); 
}

double point::lengthSq () const {
  return pow(x(), 2) + pow(y(), 2);
}

double point::length () const {
  return sqrt(lengthSq()); 
}

void point::normalise () {
  (*this) /= length(); 
}

void point::rotate (double angle) {
  // Clockwise rotation; angle is in radians
  double newx = x() * cos(angle) + y() * sin(angle);
  double newy = y() * cos(angle) - x() * sin(angle);
  x() = newx;
  y() = newy; 
}

point& point::operator+= (const point& other) {
  x() += other.x();
  y() += other.y();
}

point& point::operator-= (const point& other) {
  x() -= other.x();
  y() -= other.y();
}

point& point::operator*= (const double scale) {
  x() *= scale;
  y() *= scale; 
}

point& point::operator/= (const double scale) {
  x() /= scale;
  y() /= scale; 
}

ostream& operator<< (ostream& os, const point& p) {
  os << "(" << p.x() << ", " << p.y() << ")";
  return os; 
}

LTexture::LTexture ()
  : mTexture(0)
  , mWidth(0)
  , mHeight(0)
{}

LTexture::~LTexture(){
  // Deallocate
  free();
}

bool LTexture::loadFromFile (string path) {
#ifdef NEED_IMAGE_BLAH
  // Get rid of preexisting texture
  free();

  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
  }
  else {
    // Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    
    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else {
      // Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }
    
    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }
  
  // Return success
  mTexture = newTexture;
  return mTexture != NULL;
#endif
  return false; 
}

bool LTexture::loadFromRenderedText (string textureText, SDL_Color textColor, TTF_Font* gFont) {
  if (!gFont) return false; 
  // Get rid of preexisting texture
  free();

  // Render text surface
  SDL_Surface* textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), textColor);
  if (textSurface == NULL) {
    printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
  }
  else {
    // Get image dimensions    
    mWidth = textSurface->w;
    mHeight = textSurface->h;
    // Create texture from surface pixels
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textSurface->pixels);
    glDisable(GL_TEXTURE_2D);
    
    // Get rid of old surface
    SDL_FreeSurface(textSurface);
  }

  // Return success
  return true; 
}

void LTexture::free () {
  // Free texture if it exists - OpenGL silently ignores nonexistent ones. 
  glDeleteTextures(1, &mTexture);
  mTexture = 0; 
  mWidth = 0;
  mHeight = 0;
}

void LTexture::render (int x, int y) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mTexture);
  glColor4d(0.0, 0.0, 0.0, 0.5); 
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(x, y);
  glTexCoord2d(1.0, 0.0); glVertex2d(x + mWidth, y);
  glTexCoord2d(1.0, 1.0); glVertex2d(x + mWidth, y + mHeight);
  glTexCoord2d(0.0, 1.0); glVertex2d(x, y + mHeight);

  //glVertex2i(x, y);
  //glVertex2i(x + mWidth, y);
  //glVertex2i(x + mWidth, y + mHeight);
  //glVertex2i(x, y + mHeight);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void LTexture::setColor (Uint8 red, Uint8 green, Uint8 blue) {
  // Modulate texture rgb
  //SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode (SDL_BlendMode blending) {
  // Set blending function
  //SDL_SetTextureBlendMode(mTexture, blending);
}
		
void LTexture::setAlpha (Uint8 alpha) {
  // Modulate texture alpha
  //SDL_SetTextureAlphaMod(mTexture, alpha);
}


			
int LTexture::getWidth () {
  return mWidth;
}

int LTexture::getHeight () {
  return mHeight;
}
