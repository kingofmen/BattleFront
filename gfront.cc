#include "SDL.h"
#include "gl.h"
#include <cassert> 
#include <iostream> 
#include <sys/time.h>
#include <sys/times.h>
#include <vector>
#include <map> 

#include "Parser.hh"

#include "utils.hh" 
#include "Tile.hh" 
#include "Packet.hh" 
#include "Factory.hh" 

using namespace std; 

const int frontSize = 25; 
const int windowWidth = 1080;
const int windowHeight = 770; 
const int tileSize = 10; 
const int gridWidth = windowWidth / tileSize;
const int gridHeight = windowHeight / tileSize;

WareHouse* selectedWareHouse = 0;
bool show_cooldown = false; 
bool done = false;
bool paused = false; 

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
}

void drawRailroads () {
  glBegin(GL_LINES);
  for (Railroad::Iter r = Railroad::start(); r != Railroad::final(); ++r) {
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

void drawPackets () {
  glBegin(GL_TRIANGLES);
  for (Packet::Iter p = Packet::start(); p != Packet::final(); ++p) {
    if ((*p)->player) glColor3d(0.0, 0.0, 1.0);
    else glColor3d(1.0, 0.0, 0.0);

    glVertex2d((*p)->position.x() + 0.0, (*p)->position.y() - 2.9);
    glVertex2d((*p)->position.x() + 2.5, (*p)->position.y() + 2.1);
    glVertex2d((*p)->position.x() - 2.5, (*p)->position.y() + 2.1);
  }
  glEnd(); 
}

void initialise () {
  Object* config = processFile("config.txt"); 
  Packet::speed = config->safeGetFloat("packetSpeed", Packet::speed); 
  Vertex::troopMoveRate = config->safeGetFloat("troopMoveRate", Vertex::troopMoveRate);
  Vertex::fightRate = config->safeGetFloat("fightRate", Vertex::fightRate); 
  Vertex::minimumGarrison = config->safeGetFloat("minimumGarrison", Vertex::minimumGarrison); 
  Vertex::coolDownFactor = config->safeGetFloat("cooldown", Vertex::coolDownFactor); 
}

void handleKeyPress (SDL_KeyboardEvent& key) {
  switch (key.keysym.sym) {
  case SDLK_p: paused = !paused; break;
  case SDLK_q: done = true;  break;
  case SDLK_c: show_cooldown = !show_cooldown; break; 
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

  for (WareHouse::Iter fac = WareHouse::start(); fac != WareHouse::final(); ++fac) {
    if (!(*fac)->player) continue; 
    if (100 < click.distanceSq((*fac)->position)) continue;

    clickedWareHouse = (*fac);
    break; 
  }

  if (KEY_DOWN == keystates[BUILDKEY]) {
    if (!clickedWareHouse) {
      if (selectedWareHouse) {
	// Build new railroad and warehouse
	Tile* closest = Tile::getClosest(click, 0); 
	if (!closest) return;
	if (0.75 > closest->avgControl(true)) return; 
	
	WareHouse* house = new WareHouse(click);
	house->toCompletion = 1000;
	house->player = true;
	house->capacity = 1000; 
	
	Railroad* rail = new Railroad(selectedWareHouse, house);
	rail->upgrade();
	rail->player = true;
	
	selectedWareHouse = 0;
      }
      else {
	// Build new WareHouse, no railroad. 
      }
    }
    else {
      if (selectedWareHouse) {
	if (clickedWareHouse == selectedWareHouse) {
	  // Capacity upgrade
	}
	else {
	  // Build new railroad, or improve existing one
	  Railroad* existing = Railroad::findConnector(selectedWareHouse, clickedWareHouse); 
	  if (!existing) {
	    existing = new Railroad(selectedWareHouse, clickedWareHouse); 
	    existing->player = true;
	  }
	  existing->upgrade(); 
	}
      }
      else {
	// No interpretation of this.
      }
    }
  }
  else {
    if ((selectedWareHouse) && (clickedWareHouse == selectedWareHouse)) {
      if (SDL_BUTTON_LEFT == event.button) selectedWareHouse->toggle(); 
      else selectedWareHouse->release = !(selectedWareHouse->release); 
    }
    else selectedWareHouse = clickedWareHouse;
  }  
}

void createFactory (Object* fact) {
  Factory* fac = new Factory(point(fact->safeGetFloat("xpos"), fact->safeGetFloat("ypos")));
  fac->player = (fact->safeGetString("human", "no") == "yes");
  fac->timeToProduce = fact->safeGetInt("timeToProduce"); 
  fac->timeSinceProduction = 0; 
  fac->capacity = fact->safeGetInt("packetSize"); 
  fac->m_WareHouse.player = fac->player;
}

int main (int argc, char** argv) {
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

  int error = SDL_Init(SDL_INIT_EVERYTHING);
  assert(-1 != error); 
  SDL_Window* win = SDL_CreateWindow("BattleFront", 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (!win) {
    cout << SDL_GetError() << endl;
    return 1;
  }

  SDL_GLContext glcontext = SDL_GL_CreateContext(win);
  glClearColor(1.0, 1.0, 1.0, 0.5); 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, windowWidth, windowHeight, 0.0f, 0.0f, 1.0f);


  SDL_Event event;
  timeval prevTime;
  timeval currTime; 
  timeval passTime; 
  gettimeofday(&prevTime, NULL); 
  while (!done) {
    gettimeofday(&currTime, NULL); 
    timersub(&currTime, &prevTime, &passTime);
    int timeThisFrame = (passTime.tv_sec*1e6 + passTime.tv_usec); 
    prevTime = currTime; 
    glClear(GL_COLOR_BUFFER_BIT); 
    glColor3d(1.0, 0.0, 0.0); 
    drawTiles();
    drawFactories(); 
    drawRailroads();
    drawPackets(); 
    SDL_GL_SwapWindow(win); 

    if (!paused) {
      Vertex::fight(timeThisFrame); 
      Vertex::move(timeThisFrame); 

      for (Railroad::Iter r = Railroad::start(); r != Railroad::final(); ++r) (*r)->update(timeThisFrame); 

      int p1f = 0;
      int p2f = 0; 
      for (Factory::Iter f = Factory::start(); f != Factory::final(); ++f) {
	(*f)->produce(timeThisFrame);
	if ((*f)->tile->avgControl((*f)->player) < 0.25) (*f)->player = !(*f)->player; 
	if ((*f)->player) p1f++; else p2f++;
      }
      for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) (*w)->update(); 

      if (0 == p1f) {
	std::cout << "Computer wins\n";
	break;
      }
      if (0 == p2f) {
	std::cout << "Player wins\n";
	break;
      }
    }

    while (SDL_PollEvent(&event)){
      switch (event.type) {
      case SDL_QUIT:            done = true;                    break;
      case SDL_KEYDOWN:         handleKeyPress(event.key);      break;
      case SDL_MOUSEBUTTONDOWN: handleMouseClick(event.button); break;
      default: break; 
      }
    }
  }

  SDL_Quit();
  return 0;
}
