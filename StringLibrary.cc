#include "StringLibrary.hh"

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

void LTexture::render (double x, double y) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mTexture);
  glColor4d(0.0, 0.0, 0.0, 0.5); 
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(x, y);
  glTexCoord2d(1.0, 0.0); glVertex2d(x + mWidth, y);
  glTexCoord2d(1.0, 1.0); glVertex2d(x + mWidth, y + mHeight);
  glTexCoord2d(0.0, 1.0); glVertex2d(x, y + mHeight);
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


StringLibrary::StringLibrary (SDL_Color tc, TTF_Font* tf)
  : textColour(tc)
  , textFont(tf)
{
  registerText("0");
  registerText("1");
  registerText("2");
  registerText("3");
  registerText("4");
  registerText("5");
  registerText("6"); 
  registerText("7");
  registerText("8");
  registerText("9");
}

unsigned int StringLibrary::registerText (string txt) {
  if (index.find(txt) == index.end()) {
    LTexture* text = new LTexture();
    text->loadFromRenderedText(txt, textColour, textFont);
    index[txt] = library.size();
    library.push_back(text); 
  }
  return index[txt]; 
}

unsigned int StringLibrary::renderInt (int txt, double x, double y) {
  // Returns the width of the rendered number.
  int power = 1;
  unsigned int width = 0; 
  while (power * 10 < txt) power *= 10;
  while (0 < power) {
    int digit = txt / power;
    txt %= power;
    power /= 10;
    library[digit]->render(x + width, y);
    width += library[digit]->getWidth();
  }
  return width; 
}

unsigned int StringLibrary::renderFloat (double txt, int precision, double x, double y) {
  // Returns the width of the rendered number.
  static char buffer[1000];
  sprintf(buffer, "%.*f", precision, txt);
  unsigned int index = renderText(buffer, x, y);
  return library[index]->getWidth();
}

unsigned int StringLibrary::renderText (string txt, double x, double y) {
  unsigned int ret = registerText(txt); 
  renderText(ret, x, y);
  return ret; 
}

void StringLibrary::renderText (unsigned int idx, double x, double y) {
  library[idx]->render(x, y); 
}


