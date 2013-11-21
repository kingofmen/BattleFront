#include "Factory.hh"
#include "Packet.hh" 
#include <cmath> 

Building::Building () 
  : toCompletion(0)
{}

Railroad::Railroad (WareHouse* w1, WareHouse* w2) 
  : Building()
  , Iterable<Railroad>(this)
  , oneEnd(w1->position)
  , twoEnd(w2->position)
  , oneHouse(w1)
  , twoHouse(w2)
  , currentLoad(0)
{
  point line = twoEnd - oneEnd;
  toCompletion = (int) floor(line.length() + 0.5); 
  line.normalise(); 
  line *= 15;
  oneEnd = w1->position + line;
  twoEnd = w2->position - line;

  w1->addRailroad(this);
  w2->addRailroad(this);
}

Factory::Factory () 
  : Building()
  , Iterable<Factory>(this)
{}

WareHouse::WareHouse () 
  : Building()
  , Iterable<WareHouse>(this)
  , release(true)
  , content(0)
  , activeRail(0)
{
  capacity = 1000; 
}

bool Building::useToBuild (Packet* packet) {
  // Return true if the packet survives. 
  if (0 <= toCompletion) {
    if (toCompletion > packet->size) {
      toCompletion -= packet->size;
      delete packet;
      return false;
    }
    packet->size -= toCompletion;
    toCompletion = 0; 
  }
  return true;
}

void WareHouse::receive (Packet* packet) {
  // Four options:
  // 1. Use towards completing this building.
  // 2. Send on outgoing railroad with capacity.
  // 3. Store.
  // 4. Release. 

  if (useToBuild(packet)) return; 
  if ((activeRail) && (activeRail->canAccept(packet))) {
    activeRail->receive(packet);
    return; 
  }

  if (release) return;
  if (capacity < content + packet->getSize()) return; 
  content += packet->getSize();
  delete packet; 
}

void WareHouse::toggle () {
  if (0 == outgoing.size()) return; 
  if (!activeRail) activeRail = outgoing.front(); 
  else {
    for (Railroad::Iter r = outgoing.begin(); r != outgoing.end(); ++r) {
      if ((*r) != activeRail) continue;
      ++r;
      if (r == outgoing.end()) activeRail = 0;
      else activeRail = (*r);
      break; 
    }
  }
}

void WareHouse::update () {
  if (0 == content) return;
  if (!release) return;
  Packet* newPacket = new Packet(); 
  newPacket->player1 = player;
  newPacket->size = content;
  content = 0;
  newPacket->position = position; 
}

void Factory::produce (int elapsedTime) {
  timeSinceProduction += elapsedTime;
  if (timeSinceProduction < timeToProduce) return; 
  timeSinceProduction -= timeToProduce; 
  
  Packet* product = new Packet();
  product->size = capacity;
  product->position = position;
  product->player1 = player; 
  m_WareHouse.receive(product); 
}

bool Railroad::canAccept (Packet* packet) {
  if (0 < toCompletion) return true;
  if (capacity >= packet->size + currentLoad) return true;
  return false; 
}

void Railroad::receive (Packet* packet) {
  if (useToBuild(packet)) return; 
  
}

void Railroad::update (int elapsedTime) {
  
}
