#include "SDL.h"
#include "gl.h"
#include <cassert> 
#include <iostream> 
#include <sys/time.h>
#include <sys/times.h>
#include <vector>
#include <map> 

#include "utils.hh" 
#include "Tile.hh" 
#include "Army.hh" 
#include "Packet.hh" 
#include "Factory.hh" 

using namespace std; 

const int frontSize = 25; 
const int windowWidth = 1080;
const int windowHeight = 770; 
const int tileSize = 10; 
const int gridWidth = windowWidth / tileSize;
const int gridHeight = windowHeight / tileSize;


void drawTiles () {
  glBegin(GL_QUADS);
  for (Tile::Iter tile = Tile::start(); tile != Tile::final(); ++tile) {
    for (int i = 0; i < 4; ++i) {
      //cout << "Drawing " << (*tile) << " " << i << " " << (*tile)->corners[i] << endl; 
      glColor3d((*tile)->corners[i]->playerControl, 0.0, 1.0 - (*tile)->corners[i]->playerControl); 
      glVertex2d((*tile)->corners[i]->position.x(), (*tile)->corners[i]->position.y()); 
    }
  }
  glEnd(); 
}

void drawGrid () {
  glColor3d(1.0, 1.0, 1.0); 
  for (Tile::Iter tile = Tile::start(); tile != Tile::final(); ++tile) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; ++i) {
      glVertex2d((*tile)->corners[i]->position.x(), (*tile)->corners[i]->position.y()); 
    }
    glEnd(); 
  }
}

void drawFactories (vector<Factory>& factories) {
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < factories.size(); ++i) {
    if (factories[i].player1) glColor3d(0.0, 0.0, 1.0);
    else glColor3d(1.0, 0.0, 0.0);

    glVertex2d(factories[i].position.x() + 0.0, factories[i].position.y() + 5.8);
    glVertex2d(factories[i].position.x() + 5.0, factories[i].position.y() - 4.2);
    glVertex2d(factories[i].position.x() - 5.0, factories[i].position.y() - 4.2);
  }
  glEnd(); 
}

void drawPackets () {
  glBegin(GL_TRIANGLES);
  for (Packet::Iter p = Packet::start(); p != Packet::final(); ++p) {
    if ((*p)->player1) glColor3d(0.0, 0.0, 1.0);
    else glColor3d(1.0, 0.0, 0.0);

    glVertex2d((*p)->position.x() + 0.0, (*p)->position.y() - 2.9);
    glVertex2d((*p)->position.x() + 2.5, (*p)->position.y() + 2.1);
    glVertex2d((*p)->position.x() - 2.5, (*p)->position.y() + 2.1);
  }
  glEnd(); 
}

void drawArmies () {
  glBegin(GL_QUADS);
  for (Army::Iter army = Army::start(); army != Army::final(); ++army) {
    if ((*army)->player) glColor3d(0.0, 0.0, 1.0); 
    else glColor3d(1.0, 0.0, 0.0); 
    glVertex2d((*army)->position.x() - 4, (*army)->position.y() + 4);
    glVertex2d((*army)->position.x() + 4, (*army)->position.y() + 4);
    glVertex2d((*army)->position.x() + 4, (*army)->position.y() - 4);
    glVertex2d((*army)->position.x() - 4, (*army)->position.y() - 4);
  }
  glEnd(); 
}

int main (int argc, char** argv) {
  vector<Factory> factories; 

  Vertex** grid[gridWidth+1];
  for (int i = 0; i <= gridWidth; ++i) grid[i] = new Vertex*[gridHeight+1];
  for (int xpos = 0; xpos <= gridWidth; ++xpos) {
    for (int ypos = 0; ypos <= gridHeight; ++ypos) {
      grid[xpos][ypos] = new Vertex(point(xpos*tileSize, ypos*tileSize), (xpos < 54 ? 1.0 : (xpos == 54 ? 0.5 : 0))); 
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

  Factory fac1;
  fac1.player1 = true;
  fac1.timeToProduce = 1e6; 
  fac1.timeSinceProduction = 0;
  fac1.packetSize = 50;
  fac1.position = point(300, 400); 
  factories.push_back(fac1); 

  Factory fac2;
  fac2.player1 = false; 
  fac2.timeToProduce = 1e6; 
  fac2.timeSinceProduction = 0;
  fac2.packetSize = 50;
  fac2.position = point(800, 400); 
  factories.push_back(fac2); 

  for (int i = 0; i < frontSize-1; ++i) {
    Army* curr = new Army();
    curr->supplies = 100;
    curr->player = true; 
    curr->position = point(windowWidth/2 - 15, 1 + windowHeight*(1.0 / (frontSize-1))*i); 

    curr = new Army();
    curr->supplies = 100;
    curr->player = false; 
    curr->position = point(windowWidth/2 + 15, 1 + windowHeight*(1.0 / (frontSize-1))*i); 
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

  bool done = false;
  SDL_Event event;

  bool paused = false; 
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
    drawFactories(factories); 
    drawPackets(); 
    drawArmies(); 
    //drawGrid(); 
    SDL_GL_SwapWindow(win); 

    if (!paused) {
      // Why do it this way? Because I get errors I don't understand
      // when I just use 'erase' within the loop. Don't mess with the
      // iterators. 
      vector<Packet*> toErase;
      for (Packet::Iter pack = Packet::start(); pack != Packet::final(); ++pack) {
	if (!(*pack)->update(timeThisFrame)) continue;
	toErase.push_back(*pack); 
      }
      for (vector<Packet*>::iterator p = toErase.begin(); p != toErase.end(); ++p) {
	delete (*p);
      }
      
      for (Army::Iter army = Army::start(); army != Army::final(); ++army) (*army)->fight(timeThisFrame); 
      for (Army::Iter army = Army::start(); army != Army::final(); ++army) (*army)->influence(timeThisFrame); 

      for (int xpos = 0; xpos <= gridWidth; ++xpos) {
	for (int ypos = 0; ypos <= gridHeight; ++ypos) {
	  grid[xpos][ypos]->renormalise();
	}
      }

      for (Army::Iter army = Army::start(); army != Army::final(); ++army) (*army)->advance(timeThisFrame); 
      for (Army::Iter army = Army::start(); army != Army::final(); ++army) (*army)->updateSupplies(); 

      for (unsigned int i = 0; i < factories.size(); ++i) {
	factories[i].produce(timeThisFrame);
      }
    }

    while (SDL_PollEvent(&event)){
      switch (event.type) {
      case SDL_QUIT: done = true; break;
      case SDL_KEYDOWN: 
	if (SDLK_p == event.key.keysym.sym) paused = !paused;
	else if (SDLK_q == event.key.keysym.sym) done = true; 
	break;
      case SDL_MOUSEBUTTONDOWN: done = true; break;
      default: break; 
      }
    }
  }

  SDL_Quit();
  return 0;
}
