#include "Factory.hh"
#include "Packet.hh" 

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
{}

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

void WareHouse::receive (Packet* packet) {
  // Four options:
  // 1. Use towards completing this building.
  // 2. Send on outgoing railroad with capacity.
  // 3. Store.
  // 4. Release. 

  if (0 < toCompletion) {
    if (toCompletion > packet->size) {
      toCompletion -= packet->size;
      delete packet;
      return;
    }
    packet->size -= toCompletion;
    toCompletion = 0; 
  }

  if ((activeRail) && (activeRail->canAccept(packet))) {
    activeRail->receive(packet);
    return; 
  }

  if (release) return;
  if (capacity < content + packet->getSize()) return; 
  content += packet->getSize();
  delete packet; 
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
  if (toCompletion > 0) return true;
  if (capacity >= packet->size + currentLoad) return true;
  return false; 
}

void Railroad::receive (Packet* packet) {

}
