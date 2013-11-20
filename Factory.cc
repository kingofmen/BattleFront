#include "Factory.hh"
#include "Packet.hh" 

vector<Factory*> Factory::allFactories; 
vector<WareHouse*> WareHouse::allWareHouses; 

Building::Building () 
  : toCompletion(0)
{}

Building::~Building () {} 

Factory::Factory () 
  : Building()
{
  allFactories.push_back(this); 
}

WareHouse::WareHouse () 
  : Building()
  , release(true)
  , content(0)
  , activeRail(0)
{
  allWareHouses.push_back(this);
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

Factory::~Factory () {
  for (unsigned int i = 0; i < allFactories.size(); ++i) {
    if (allFactories[i] != this) continue;
    allFactories[i] = allFactories.back();
    break;
  }
  allFactories.pop_back(); 
}

WareHouse::~WareHouse () {
  for (unsigned int i = 0; i < allWareHouses.size(); ++i) {
    if (allWareHouses[i] != this) continue;
    allWareHouses[i] = allWareHouses.back();
    break;
  }
  allWareHouses.pop_back(); 
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

Railroad::Railroad () 
  : Building()
  , currentLoad(0)
{}

bool Railroad::canAccept (Packet* packet) {
  if (toCompletion > 0) return true;
  if (capacity >= packet->size + currentLoad) return true;
  return false; 
}

void Railroad::receive (Packet* packet) {

}
