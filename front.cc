#include <iostream>
#include <cassert> 
#include <cstdio> 
#include <string> 
#include <sys/time.h>
#include <sys/times.h>
#include <curses.h> 
#include <cstdlib>
#include <vector> 

using namespace std; 
timeval startTime, stopTime, totalTime;

enum GameState {Fight = 1, VictoryPlayer = 2, VictoryAi = 4}; 

struct Army {
  double supplies1; 
  double supplies2; 
  int position;
  int supplied; 
};

struct SupplyPacket {
  double size;
  int xpos;
  int ypos; 
  bool player; 
};

const int frontSize = 25;

int main (int argc, char** argv) {
  vector<Army> frontpos(frontSize); 
  for (int i = 0; i < frontSize; ++i) {
    frontpos[i].supplies1 = 100;
    frontpos[i].supplies2 = 100;
    frontpos[i].position = 25; 
    frontpos[i].supplied = Fight; 
  }

  vector<SupplyPacket> packets; 

  GameState victory = Fight; 

  initscr(); 
  start_color(); 
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_WHITE);
  init_pair(3, COLOR_BLUE, COLOR_WHITE);
  init_pair(4, COLOR_MAGENTA, COLOR_WHITE);
  color_set(COLOR_PAIR(2), 0); 

  noecho(); 
  timeout(0); 
  cbreak(); 

  int playerFactory = 12; 

  while (true) {
    gettimeofday(&startTime, NULL);

    move(playerFactory, 0);
    printw(" "); 

    int input = getch(); 
    while (input != ERR) {
      playerFactory++;
      if (playerFactory >= frontSize) playerFactory = 0; 
      input = getch(); 
    }

    move(playerFactory, 0);
    printw("X"); 


    for (int i = 0; i < frontSize; ++i) {
      move(i, frontpos[i].position);
      frontpos[i].supplied = Fight; 
      printw(" ");
    }

    int lucky = playerFactory; 
    SupplyPacket nPacket1;
    nPacket1.size = 50;
    nPacket1.xpos = 1;
    nPacket1.ypos = lucky;
    nPacket1.player = true;
    packets.push_back(nPacket1);

    lucky = rand()%frontSize;
    SupplyPacket nPacket2;
    nPacket2.size = 50;
    nPacket2.xpos = 49;
    nPacket2.ypos = lucky;
    nPacket2.player = false; 
    packets.push_back(nPacket2);

    for (vector<SupplyPacket>::iterator packet = packets.begin(); packet != packets.end(); ++packet) {
      if ((*packet).xpos == frontpos[(*packet).ypos].position) {
	if ((*packet).player) {
	  frontpos[(*packet).ypos].supplies1 += (*packet).size;
	  frontpos[(*packet).ypos].supplied |= VictoryPlayer;
	}
	else {
	  frontpos[(*packet).ypos].supplies2 += (*packet).size;
	  frontpos[(*packet).ypos].supplied |= VictoryAi;
	}
	packet = packets.erase(packet); 
	continue; 
      }

      move((*packet).ypos, (*packet).xpos);
      printw(" ");

      if ((*packet).player) (*packet).xpos++;
      else (*packet).xpos--; 

      if ((*packet).xpos == frontpos[(*packet).ypos].position) {
	if ((*packet).player) {
	  frontpos[(*packet).ypos].supplies1 += (*packet).size;
	  frontpos[(*packet).ypos].supplied |= VictoryPlayer;
	}
	else {
	  frontpos[(*packet).ypos].supplies2 += (*packet).size;
	  frontpos[(*packet).ypos].supplied |= VictoryAi;
	}
	packet = packets.erase(packet); 
      }
      else {
	move((*packet).ypos, (*packet).xpos);
	printw(".");
      }

    }

    for (int i = 0; i < frontSize; ++i) {
      if      (frontpos[i].supplies1 >= frontpos[i].supplies2 * 1.25) frontpos[i].position++;
      else if (frontpos[i].supplies2 >= frontpos[i].supplies1 * 1.25) frontpos[i].position--;
      if (frontpos[i].position <= 0) victory = VictoryAi;
      else if (frontpos[i].position >= 50) victory = VictoryPlayer; 

      frontpos[i].supplies1 *= 0.9;
      frontpos[i].supplies2 *= 0.9;
    }

    if (victory != Fight) break; 

    for (int i = 0; i < frontSize; ++i) {
      switch (frontpos[i].supplied) {
      default: 
      case Fight:         attron(COLOR_PAIR(1)); break;
      case VictoryPlayer: attron(COLOR_PAIR(2)); break;
      case VictoryAi:     attron(COLOR_PAIR(3)); break;
      case (VictoryPlayer & VictoryAi): attron(COLOR_PAIR(4)); break;
      }
      
      move(i, frontpos[i].position);
      printw("F");
    }

    refresh(); 
    while (true) {
      gettimeofday(&stopTime, NULL);
      timersub(&stopTime, &startTime, &totalTime);
      if (totalTime.tv_sec*1000000 + totalTime.tv_usec > 400000) break; 
    }
  }
  endwin(); 

  if (VictoryPlayer == victory) cout << "Player wins!\n";
  else cout << "Computer wins!\n"; 

  return 0;
}




