#include "SDL.h"
#include "gl.h"
#include <cassert> 
#include <iostream> 
#include <sys/time.h>
#include <sys/times.h>
#include <vector>
#include <map> 

#include "utils.hh" 
#include "Army.hh" 
#include "Packet.hh" 
#include "Factory.hh" 

using namespace std; 

const int frontSize = 25; 
const int windowWidth = 1080;
const int windowHeight = 768; 

struct Shape {
  vector<point*> fixPoints;
};

struct Front {
  vector<Army*> armies; 
  typedef vector<Army*>::iterator Iter;
  Iter begin () {return armies.begin();}
  Iter final () {return armies.end();} 

  void fight (int elapsedTime);
};

void Front::fight (int elapsedTime) {
  for (unsigned int i = 0; i < armies.size(); ++i) {
    double advance = armies[i]->fight(elapsedTime);
    armies[i]->advance = armies[i]->position; 
    if (fabs(advance) < 0.01) continue; 
    if ((0 == i) || (armies.size()-1 == i)) {
      
    }
    else {
      point baseline = armies[i+1]->position; 
      baseline -= armies[i-1]->position; 
      baseline.rotate(0.5*M_PI);
      baseline.normalise();
      baseline *= advance;
      armies[i]->position += baseline; 
      baseline *= 6; 
      baseline += armies[i]->position; 
      armies[i]->advance = baseline;
    }
  }

  // Straighten out wedges
  static const double thirty = 0.16667*M_PI; 
  for (unsigned int i = 1; i < armies.size()-1; ++i) {
    point oneDirection = armies[i-1]->position - armies[i]->position;
    point twoDirection = armies[i+1]->position - armies[i]->position;
    double angle = oneDirection.angle(twoDirection);
    if (angle > thirty) continue;
    
    point targetPt = armies[i-1]->position;
    targetPt += armies[i+1]->position;
    targetPt *= 0.5; 
    targetPt -= armies[i]->position; 
    targetPt *= 0.9*(1.05 - angle / thirty);
    armies[i]->position += targetPt; 
  }

  for (unsigned int i = 1; i < armies.size(); ++i) {
    point distance = armies[i]->position;
    distance -= armies[i-1]->position;
    double dist = distance.length(); 
    if (dist > 50) {
      // Create a new army
      distance *= 0.5;
      distance += armies[i-1]->position;
      Army* newArmy = new Army(); 
      newArmy->position = distance; 
      newArmy->advance  = distance; 
      newArmy->supplies1  = 0.33 * armies[i]->supplies1;
      newArmy->supplies2  = 0.33 * armies[i]->supplies2;
      newArmy->supplies1 += 0.33 * armies[i-1]->supplies1;
      newArmy->supplies2 += 0.33 * armies[i-1]->supplies2;
      
      armies[i]->supplies1 -= 0.33 * armies[i]->supplies1;
      armies[i]->supplies2 -= 0.33 * armies[i]->supplies2;
      armies[i-1]->supplies1 -= 0.33 * armies[i-1]->supplies1;
      armies[i-1]->supplies2 -= 0.33 * armies[i-1]->supplies2;
      
      unsigned int j = armies.size();
      for (armies.push_back(0); j > i; --j) {
	armies[j] = armies[j-1];
      }
      armies[i] = newArmy;
      ++i; 
    }
    else if (dist < 5) {
      // Absorb previous army into this one
      armies[i]->supplies1 += armies[i-1]->supplies1;
      armies[i]->supplies2 += armies[i-1]->supplies2;
      delete armies[i-1]; 
      for (int j = i-1; j < armies.size()-1; ++j) {
	armies[j] = armies[j+1]; 
      }
      armies.pop_back(); 
    }
  }
}

void drawFactories (vector<Factory>& factories) {
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < factories.size(); ++i) {
    if (factories[i].player1) glColor3d(1.0, 0.0, 0.0);
    else glColor3d(0.0, 0.0, 1.0);

    glVertex2d(factories[i].position.x() + 0.0, factories[i].position.y() + 5.8);
    glVertex2d(factories[i].position.x() + 5.0, factories[i].position.y() - 4.2);
    glVertex2d(factories[i].position.x() - 5.0, factories[i].position.y() - 4.2);
  }
  glEnd(); 
}

void drawFront (Front& theFront) {
  glLineWidth(3); 
  glColor3d(1.0, 0.0, 0.0); 
  glBegin(GL_LINE_STRIP);
  for (Front::Iter army = theFront.begin(); army != theFront.final(); ++army) {
    glVertex2d((*army)->position.x(), (*army)->position.y()); 
  }
  glEnd(); 

  glColor3d(0.0, 0.0, 1.0); 
  glBegin(GL_LINES);
  for (Front::Iter army = theFront.begin(); army != theFront.final(); ++army) {
    glVertex2d((*army)->position.x(), (*army)->position.y()); 
    glVertex2d((*army)->advance.x(), (*army)->advance.y()); 
  }
  glEnd(); 
}

void drawPackets (vector<Packet*>& packets) {
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < packets.size(); ++i) {
    if (packets[i]->player1) glColor3d(1.0, 0.0, 0.0);
    else glColor3d(0.0, 0.0, 1.0);

    glVertex2d(packets[i]->position.x() + 0.0, packets[i]->position.y() - 2.9);
    glVertex2d(packets[i]->position.x() + 2.5, packets[i]->position.y() + 2.1);
    glVertex2d(packets[i]->position.x() - 2.5, packets[i]->position.y() + 2.1);
  }
  glEnd(); 
}


int main (int argc, char** argv) {
  vector<Factory> factories; 
  vector<Packet*> packets; 
  Shape continent;
  continent.fixPoints.push_back(new point(0, 0));
  continent.fixPoints.push_back(new point(windowWidth, 0));
  continent.fixPoints.push_back(new point(0, windowHeight));
  continent.fixPoints.push_back(new point(windowWidth, windowHeight));

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

  Front theFront;
  for (int i = 0; i < frontSize; ++i) {
    Army* curr = new Army();
    curr->supplies1 = 100;
    curr->supplies2 = 100;
    curr->position = point(windowWidth/2, windowHeight*(1.0 / (frontSize-1))*i); 
    theFront.armies.push_back(curr); 
  }

  Shape westHalf;
  westHalf.fixPoints.push_back(continent.fixPoints[0]);
  for (Front::Iter army = theFront.begin(); army != theFront.final(); ++army) {
    westHalf.fixPoints.push_back(&((*army)->position));
  }
  westHalf.fixPoints.push_back(continent.fixPoints[3]);

  Shape eastHalf;
  eastHalf.fixPoints.push_back(continent.fixPoints[1]);
  for (Front::Iter army = theFront.begin(); army != theFront.final(); ++army) {
    eastHalf.fixPoints.push_back(&((*army)->position));
  }
  eastHalf.fixPoints.push_back(continent.fixPoints[2]);

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
    drawFront(theFront); 
    drawFactories(factories); 
    drawPackets(packets); 
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
      
      for (unsigned int i = 0; i < factories.size(); ++i) {
	factories[i].produce(timeThisFrame, packets);
      }
      
      theFront.fight(timeThisFrame); 
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
