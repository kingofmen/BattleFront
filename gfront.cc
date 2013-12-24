#ifdef WINDOWS
#include <windows.h> 
#include <gl/gl.h>
#define SDL_MAIN_HANDLED
#else
#include "gl.h"
#include <sys/times.h>
#endif
#include "SDL.h"
#include "SDL_ttf.h"
#include <cassert> 
#include <iostream> 
#include <sys/time.h>
#include <vector>
#include <map> 

#include "Parser.hh"

#include "utils.hh" 
#include "Tile.hh" 
#include "Packet.hh" 
#include "Factory.hh" 
#include "StringLibrary.hh" 

using namespace std; 

const int frontSize = 25; 
const int windowWidth = 1080;
const int windowHeight = 770; 
const int tileSize = 10; 
const int gridWidth = windowWidth / tileSize;
const int gridHeight = windowHeight / tileSize;

WareHouse* selectedWareHouse = 0;
bool show_cooldown = false;
bool show_aidebug = false;
StringLibrary* bigLetters = 0;
StringLibrary* smallLetters = 0;
enum GameState {Running, Paused, Victory, Defeat, Quit};
GameState currentState = Running; 

void drawTiles () {
  static const double invMaxTroops = 0.2; 

  glBegin(GL_QUADS);
  for (Tile::Iter tile = Tile::start(); tile != Tile::final(); ++tile) {
    for (int i = 0; i < 4; ++i) {
      if ((*tile)->corners[i]->player) glColor3d(min(1.0, (*tile)->corners[i]->troops * invMaxTroops), 0.0, 0.0);
      else glColor3d(0.0, 0.0, min(1.0, (*tile)->corners[i]->troops*invMaxTroops)); 
      glVertex2d((*tile)->corners[i]->position.x(), (*tile)->corners[i]->position.y()); 
    }
  }
  glEnd(); 

  if (show_cooldown) {
    glBegin(GL_POINTS);
    for (Vertex::Iter v = Vertex::start(); v != Vertex::final(); ++v) {
      if (0 >= (*v)->cooldown) continue; 
      glColor3d(0.002*(*v)->cooldown, 0.002*(*v)->cooldown, 0.002*(*v)->cooldown); 
      glVertex2d((*v)->position.x(), (*v)->position.y());
    }
    glEnd(); 
  }
}

void drawFactories () {
  glBegin(GL_TRIANGLES);

  for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) {
    if ((*w)->player) glColor3d(0.0, 0.0, 0.2 + 0.8*(*w)->getCompFraction());
    else glColor3d(0.2 + 0.8*(*w)->getCompFraction(), 0.0, 0.0);
    
    glVertex2d((*w)->position.x() + 0.0, (*w)->position.y() + 9.3 + 1.5);
    glVertex2d((*w)->position.x() + 8.0, (*w)->position.y() - 6.7 + 1.5);
    glVertex2d((*w)->position.x() - 8.0, (*w)->position.y() - 6.7 + 1.5);
  }
  
  for (Factory::Iter f = Factory::start(); f != Factory::final(); ++f) {
    if ((*f)->player) glColor3d(0.0, 0.0, 1.0);
    else glColor3d(1.0, 0.0, 0.0);

    glVertex2d((*f)->position.x() + 0.0, (*f)->position.y() - 9.3 - 1.5);
    glVertex2d((*f)->position.x() - 8.0, (*f)->position.y() + 6.7 - 1.5);
    glVertex2d((*f)->position.x() + 8.0, (*f)->position.y() + 6.7 - 1.5);
  }
  glEnd(); 

  if (selectedWareHouse) {
    glColor3d(1.0, 1.0, 1.0); 
    glBegin(GL_LINE_STRIP);
    glVertex2d(selectedWareHouse->position.x() - 11, selectedWareHouse->position.y() - 11);
    glVertex2d(selectedWareHouse->position.x() + 13, selectedWareHouse->position.y() - 11);
    glVertex2d(selectedWareHouse->position.x() + 13, selectedWareHouse->position.y() + 13);
    glVertex2d(selectedWareHouse->position.x() - 11, selectedWareHouse->position.y() + 13);
    glVertex2d(selectedWareHouse->position.x() - 11, selectedWareHouse->position.y() - 11);
    glEnd();
  }

  if (show_aidebug) {
    for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) {
      if ((*w)->player) continue;
      smallLetters->renderInt((*w)->m_ai->threatLevel, (*w)->position.x(), (*w)->position.y()); 
    }
  }
    
}

void drawRailroads () {
  glBegin(GL_LINES);
  for (Railroad::Iter r = Railroad::start(); r != Railroad::final(); ++r) {
    glColor3d(0.2, 0.2, 0.2);
    glVertex2d((*r)->oneEnd.x(), (*r)->oneEnd.y());
    glVertex2d((*r)->twoEnd.x(), (*r)->twoEnd.y());

    glColor3d(1.0, 1.0, 1.0); 
    point end = (*r)->twoEnd - (*r)->oneEnd; 
    end *= (*r)->getCompFraction();
    end += (*r)->oneEnd;
    glVertex2d((*r)->oneEnd.x(), (*r)->oneEnd.y());
    glVertex2d(end.x(), end.y()); 
    if ((*r)->oneHouse->activeRail == (*r)) {
      glVertex2d((*r)->oneEnd.x(), (*r)->oneEnd.y());
      glVertex2d((*r)->oneHouse->position.x(), (*r)->oneHouse->position.y());
    }
    if ((*r)->twoHouse->activeRail == (*r)) {
      glVertex2d((*r)->twoEnd.x(), (*r)->twoEnd.y());
      glVertex2d((*r)->twoHouse->position.x(), (*r)->twoHouse->position.y());
    }
  }
  glEnd(); 
}

void drawLocomotives () {
  glBegin(GL_TRIANGLES);
  for (Locomotive::Iter p = Locomotive::start(); p != Locomotive::final(); ++p) {
    glColor3d(1.0, 1.0, 1.0);
    glVertex2d((*p)->position.x() + 0.0, (*p)->position.y() - 2.9);
    glVertex2d((*p)->position.x() + 2.5, (*p)->position.y() + 2.1);
    glVertex2d((*p)->position.x() - 2.5, (*p)->position.y() + 2.1);
  }  
  glEnd(); 
}

void initialise () {
  Object* config = processFile("config.txt"); 
  Railroad::speed = config->safeGetFloat("packetSpeed", Railroad::speed); 
  Vertex::troopMoveRate = config->safeGetFloat("troopMoveRate", Vertex::troopMoveRate);
  Vertex::fightRate = config->safeGetFloat("fightRate", Vertex::fightRate); 
  Vertex::minimumGarrison = config->safeGetFloat("minimumGarrison", Vertex::minimumGarrison); 
  Vertex::coolDownFactor = config->safeGetFloat("cooldown", Vertex::coolDownFactor); 
  Vertex::attritionRate = config->safeGetFloat("attrition", Vertex::attritionRate);
  WareHouse::newBuildSize = config->safeGetInt("newDepotSize", WareHouse::newBuildSize);

  Object* ai = config->getNeededObject("ai");
  WarehouseAI::defcon5 = ai->safeGetInt("defcon5", WarehouseAI::defcon5);
  WarehouseAI::defcon4 = ai->safeGetInt("defcon4", WarehouseAI::defcon4);
  WarehouseAI::defcon3 = ai->safeGetInt("defcon3", WarehouseAI::defcon3);
  WarehouseAI::defcon2 = ai->safeGetInt("defcon2", WarehouseAI::defcon2);  
}

void handleKeyPress (SDL_KeyboardEvent& key) {
  switch (key.keysym.sym) {
  case SDLK_p:
    if (Running == currentState) currentState = Paused;
    else if (Paused == currentState) currentState = Running; 
    break;
  case SDLK_q: currentState = Quit;  break;
  case SDLK_c: show_cooldown = !show_cooldown; break;
  case SDLK_a: show_aidebug = !show_aidebug; break; 
  case SDLK_ESCAPE: selectedWareHouse = 0; break;
  default: break; 
  }
}

void handleMouseClick (const SDL_MouseButtonEvent& event) {
  point click(event.x, event.y); 
  WareHouse* clickedWareHouse = 0; 

  static int numkeys[1]; 
  static const Uint8* keystates = SDL_GetKeyboardState(numkeys);
  static const Uint8 KEY_DOWN = 1;
  static const int BUILDKEY = SDL_SCANCODE_LCTRL;
  static const int SHIFTKEY = SDL_SCANCODE_LSHIFT; 

  WareHouse* closest = 0;
  for (WareHouse::Iter fac = WareHouse::start(); fac != WareHouse::final(); ++fac) {
    if (!(*fac)->player) continue; 
    if ((!closest) || (click.distanceSq(closest->position) > click.distanceSq((*fac)->position))) closest = (*fac); 
    if (100 < click.distanceSq((*fac)->position)) continue;

    clickedWareHouse = (*fac);
    break; 
  }

  if (KEY_DOWN == keystates[BUILDKEY]) {
    if (!clickedWareHouse) {
      // C-click on 'wilderness': Build new WareHouse. 
      if (1600 > click.distanceSq(closest->position)) return; // No warehouse building this close to others 

      // Check for enemy control
      Tile* closest = Tile::getClosest(click, 0); 
      if (!closest) return;
      if (0.75 > closest->avgControl(true)) return; 

      // Check if new WareHouse splits a railroad in two
      Railroad* toSplit = 0; 
      for (Railroad::Iter r = Railroad::start(); r != Railroad::final(); ++r) {
	point xaxis = (*r)->twoEnd - (*r)->oneEnd;
	point origin = (*r)->oneEnd; 
	xaxis.normalise(); 
	point clickprime = click - origin;
	double radius = clickprime.length();
	double angle = xaxis.angle(clickprime); 
	double ydistance = radius * sin(angle); 
	if (12 < fabs(ydistance)) continue; 
	double xdistance = radius * cos(angle); 
	if (xdistance > (*r)->getLength()) continue;
	if (xdistance < 0) continue; 
	click = origin + xaxis*xdistance; 
	toSplit = (*r);
      }

      WareHouse* house = new WareHouse(click);
      house->player = true;

      if (selectedWareHouse) {
	selectedWareHouse->connect(house); 
	selectedWareHouse = 0;
      }

      if (toSplit) {
	WareHouse* oldTerminus = toSplit->twoHouse; 
	Railroad* newRail = new Railroad(house, oldTerminus);
	int oldLength = toSplit->getLength(); 
	oldTerminus->replaceRail(toSplit, newRail); 
	house->addRailroad(toSplit); 
	house->toggleRail(); 
	int locos = toSplit->capacity;
	newRail->capacity = (locos/2);
	toSplit->capacity = (locos/2) + (locos%2); 
	toSplit->twoHouse = house; 
	toSplit->calcEnds(); 
	
	if (0 < toSplit->toCompletion) {
	  int newLength = toSplit->getLength(); 
	  int completed = oldLength - toSplit->toCompletion; 
	  if (completed > newLength) {
	    toSplit->toCompletion = 0;
	    newRail->toCompletion -= (completed - newLength);
	  }
	  else toSplit->toCompletion -= (oldLength - newLength); 
	}
	else newRail->toCompletion = 0; 
      }
    }
    else {
      // C-click on existing WareHouse. 
      if (selectedWareHouse) {
	// With prior selection of click target: New Locomotive.
	if (clickedWareHouse == selectedWareHouse) {
	  for (Factory::Iter f = Factory::start(); f != Factory::final(); ++f) {
	    if (clickedWareHouse != &(*f)->m_WareHouse) continue;
	    (*f)->orderLoco(); 
	    break;
	  }
	}
	else {
	  // With prior selection of different WareHouse: New railroad.
	  selectedWareHouse->connect(clickedWareHouse);
	}
      }
      else {
	// No interpretation of C-click on existing WareHouse without prior selection. 
      }
    }
  }
  else if (KEY_DOWN == keystates[SHIFTKEY]) {
    if (!clickedWareHouse) return; // No interpretation of 'wilderness' shift-click.
    if ((selectedWareHouse) && (selectedWareHouse != clickedWareHouse)) {
      // Shift-click on different WareHouse: Transfer locomotive if possible.
      selectedWareHouse->sendLoco(clickedWareHouse);
      return;
    }
    if (SDL_BUTTON_LEFT == event.button) {
      // Shift-left-click: Upgrade
    }
    else {
      // Shift-right-click: Toggle hold state backwards. 
      selectedWareHouse->toggleHoldState(true);
    }
  }
  else {
    if (!clickedWareHouse) {
      // Click in wilderness - deselect. 
      selectedWareHouse = 0;
      return;
    }
    if (selectedWareHouse == clickedWareHouse) {
      if (SDL_BUTTON_LEFT == event.button) {
	// Left-click on selected warehouse: Toggle outgoing railroad. 
	selectedWareHouse->toggleRail();
      }
      else {
	// Right-click on selected: Toggle hold state forwards. 
	selectedWareHouse->toggleHoldState(false);
      }
    }
    // Left-click on new WareHouse: Select it. 
    else selectedWareHouse = clickedWareHouse;
  }  
}

void createFactory (Object* fact) {
  Factory* fac = new Factory(point(fact->safeGetFloat("xpos"), fact->safeGetFloat("ypos")));
  fac->player = (fact->safeGetString("human", "no") == "yes");
  fac->timeToProduce = fact->safeGetInt("timeToProduce"); 
  fac->timeSinceProduction = 0; 
  fac->production.add(Packet::Manpower, fact->safeGetInt("manpower"));
  fac->production.add(Packet::Gasoline, fact->safeGetInt("gasoline"));
  fac->production.add(Packet::Materiel, fact->safeGetInt("materiel"));   
  fac->m_WareHouse.player = fac->player;
  fac->m_WareHouse.toCompletion = 0; 
}

int main (int argc, char* argv[]) {
  SDL_SetMainReady();  
  initialise(); 

  vector<Factory*> factories; 

  Vertex** grid[gridWidth+1];
  for (int i = 0; i <= gridWidth; ++i) grid[i] = new Vertex*[gridHeight+1];
  for (int xpos = 0; xpos <= gridWidth; ++xpos) {
    for (int ypos = 0; ypos <= gridHeight; ++ypos) {
      grid[xpos][ypos] = new Vertex(point(xpos*tileSize, ypos*tileSize), xpos < 54, 1);
      //if ((xpos == 52) && (ypos == 20)) grid[xpos][ypos]->debug = true; 
      //if ((xpos == 51) && (ypos == 20)) grid[xpos][ypos]->debug = true; 
      //if ((xpos == 53) && (ypos == 20)) grid[xpos][ypos]->debug = true; 
      if (xpos > 0) {
	grid[xpos-0][ypos]->setNeighbour(Vertex::West, grid[xpos-1][ypos]);
	grid[xpos-1][ypos]->setNeighbour(Vertex::East, grid[xpos-0][ypos]);
      }
      if (ypos > 0) {
	grid[xpos][ypos-0]->setNeighbour(Vertex::North, grid[xpos][ypos-1]);
	grid[xpos][ypos-1]->setNeighbour(Vertex::South, grid[xpos][ypos-0]);
      }
    }
  }

  for (int xpos = 0; xpos < gridWidth; ++xpos) {
    for (int ypos = 0; ypos < gridHeight; ++ypos) {
      Tile* curr = new Tile(grid[xpos+0][ypos+0],
			    grid[xpos+1][ypos+0],
			    grid[xpos+1][ypos+1],
			    grid[xpos+0][ypos+1]);
      
    }
  }

  
  Object* scenario = processFile("scenario.txt");
  objvec facts = scenario->getValue("factory"); 
  for (objiter fact = facts.begin(); fact != facts.end(); ++fact) {
    createFactory(*fact); 
  }

  // Do not use INIT_EVERYTHING because that includes haptic, and gives me trouble on Windows. 
  int error = SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS); 
  if (-1 == error) {
    cout << SDL_GetError() << endl;
    exit(1); 
  }
  SDL_Window* win = SDL_CreateWindow("BattleFront", 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (!win) {
    cout << SDL_GetError() << endl;
    assert(win);    
    return 1;
  }

  error = TTF_Init();
  if (-1 == error) {
    cout << TTF_GetError() << endl;
    return 1; 
  }		

  SDL_GLContext glcontext = SDL_GL_CreateContext(win);
  assert(glcontext); 
  glClearColor(1.0, 1.0, 1.0, 0.5); 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, windowWidth, windowHeight, 0.0f, 0.0f, 1.0f);

  // With these three I get nice white text with transparency where there
  // isn't text. I wish I had a model for why REPLACE works and MODULATE and the
  // two others don't. 
  glEnable(GL_BLEND);    
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  TTF_Font* bigFont = TTF_OpenFont("SourceSansPro-Regular.ttf", 32); 
  if (!bigFont) {
    cout << "Failed to load font SourceSansPro-Regular.ttf: " << TTF_GetError() << endl;
    return 1;
  }

  TTF_Font* smallFont = TTF_OpenFont("SourceSansPro-Regular.ttf", 14); 
  if (!smallFont) {
    cout << "Failed to load font SourceSansPro-Regular.ttf: " << TTF_GetError() << endl;
    return 1;
  }

  SDL_Color textColour = {255, 255, 255, 255};  
  bigLetters   = new StringLibrary(textColour, bigFont);
  smallLetters = new StringLibrary(textColour, smallFont);

  unsigned int STR_VICTORY = bigLetters->registerText("VICTORY!");
  unsigned int STR_DEFEAT  = bigLetters->registerText("DEFEAT!");
  
  currentState = Running; 
  SDL_Event event;
  timeval prevTime;
  timeval currTime; 
  timeval passTime; 
  gettimeofday(&prevTime, NULL);
  while (Quit != currentState) {
    gettimeofday(&currTime, NULL); 
    timersub(&currTime, &prevTime, &passTime);
    int timeThisFrame = (passTime.tv_sec*1e6 + passTime.tv_usec);
    if (timeThisFrame < 18000) continue;
    prevTime = currTime; 
    glClear(GL_COLOR_BUFFER_BIT); 
    glColor3d(1.0, 0.0, 0.0); 
    drawTiles();
    drawFactories(); 
    drawRailroads();
    drawLocomotives();
    if (Victory == currentState) bigLetters->renderText(STR_VICTORY, 470, 300);
    else if (Defeat == currentState) bigLetters->renderText(STR_DEFEAT, 470, 300);
    
    SDL_GL_SwapWindow(win); 

    if (Running == currentState) {
      Vertex::attrite(timeThisFrame); 
      Vertex::fight(timeThisFrame); 
      Vertex::move(timeThisFrame);      

      for (Railroad::Iter r = Railroad::start(); r != Railroad::final(); ++r) (*r)->update(timeThisFrame); 
      WarehouseAI::globalAI(); 
      
      int p1f = 0;
      int p2f = 0; 
      for (Factory::Iter f = Factory::start(); f != Factory::final(); ++f) {
	(*f)->produce(timeThisFrame);
	if ((*f)->tile->avgControl((*f)->player) < 0.25) (*f)->player = !(*f)->player; 
	if ((*f)->player) p1f++; else p2f++;
      }
      for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) (*w)->update(timeThisFrame); 
      
      if (0 == p1f) {
	currentState = Defeat;
      }
      if (0 == p2f) {
	currentState = Victory;
      }
    }

    while (SDL_PollEvent(&event)){
      switch (event.type) {
      case SDL_QUIT:            currentState = Quit;            break;
      case SDL_KEYDOWN:         handleKeyPress(event.key);      break;
      case SDL_MOUSEBUTTONDOWN: handleMouseClick(event.button); break;
      default: break; 
      }
    }
  }

  for (Tile::Iter tile = Tile::start(); tile != Tile::final(); ++tile) delete (*tile);
  for (Vertex::Iter v = Vertex::start(); v != Vertex::final(); ++v) delete (*v); 
  
  SDL_Quit();


  return 0;
}


