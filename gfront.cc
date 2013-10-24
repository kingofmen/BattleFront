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

void drawTiles () {
  glBegin(GL_QUADS);
  for (Tile::Iter tile = Tile::start(); tile != Tile::final(); ++tile) {
    for (int i = 0; i < 4; ++i) {
      glColor3d((*tile)->corners[i]->playerControl, 0.0, 1.0 - (*tile)->corners[i]->playerControl); 
      glVertex2d((*tile)->corners[i]->position.x(), (*tile)->corners[i]->position.y()); 
    }
  }
  glEnd(); 
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

void drawPackets (vector<Packet*>& packets) {
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < packets.size(); ++i) {
    if (packets[i]->player1) glColor3d(0.0, 0.0, 1.0);
    else glColor3d(1.0, 0.0, 0.0);

    glVertex2d(packets[i]->position.x() + 0.0, packets[i]->position.y() - 2.9);
    glVertex2d(packets[i]->position.x() + 2.5, packets[i]->position.y() + 2.1);
    glVertex2d(packets[i]->position.x() - 2.5, packets[i]->position.y() + 2.1);
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
  vector<Packet*> packets; 

  const int tileSize = 10; 
  const int gridWidth = windowWidth / tileSize;
  const int gridHeight = windowHeight / tileSize;

  Vertex* grid[(gridWidth+1)*(gridHeight+1)];
  for (int xpos = 0; xpos <= gridWidth; ++xpos) {
    for (int ypos = 0; ypos <= gridHeight; ++ypos) {
      grid[gridWidth*ypos + xpos] = new Vertex(point(xpos*10, ypos*10), (xpos < 54 ? 1.0 : (xpos == 54 ? 0.5 : 0))); 
    }
  }

  for (int xpos = 0; xpos < gridWidth; ++xpos) {
    for (int ypos = 0; ypos < gridHeight; ++ypos) {
      Tile* curr = new Tile(grid[(ypos+0)*gridWidth+(xpos+0)],
			    grid[(ypos+0)*gridWidth+(xpos+1)],
			    grid[(ypos+1)*gridWidth+(xpos+1)],
			    grid[(ypos+1)*gridWidth+(xpos+0)]);
	
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

  for (int i = 0; i < frontSize; ++i) {
    Army* curr = new Army();
    curr->supplies = 100;
    curr->player = true; 
    curr->position = point(windowWidth/2 - 15, 5 + windowHeight*(1.0 / (frontSize-1))*i); 

    curr = new Army();
    curr->supplies = 100;
    curr->player = false; 
    curr->position = point(windowWidth/2 + 15, 5 + windowHeight*(1.0 / (frontSize-1))*i); 
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
    drawPackets(packets); 
    drawArmies(); 
    SDL_GL_SwapWindow(win); 

    if (!paused) {
      // Why do it this way? Because I get errors I don't understand
      // when I just use 'erase' within the loop. Don't mess with the
      // iterators. 
      vector<unsigned int> toErase;
      for (unsigned int i = 0; i < packets.size(); ++i) {
	if (!packets[i]->update(timeThisFrame)) continue;
	toErase.push_back(i); 
      }
      for (int i = -1 + toErase.size(); i >= 0; --i) {
	delete packets[toErase[i]];
	packets[toErase[i]] = packets.back(); 
	packets.pop_back(); 
      }
      
      for (Army::Iter army = Army::start(); army != Army::final(); ++army) {
	(*army)->update(timeThisFrame); 
      }

      for (unsigned int i = 0; i < factories.size(); ++i) {
	factories[i].produce(timeThisFrame, packets);
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
