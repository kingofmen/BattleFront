#include "Factory.hh"
#include "Packet.hh" 
#include "Tile.hh" 
#include <cmath> 
#include <cassert> 

double Railroad::speed = 0.0001;
int WareHouse::newBuildSize = 500; 

Building::Building (point p) 
  : position(p)
  , capacity(0)
  , toCompletion(0)
{
  tile = Tile::getClosest(position, 0); 
}

Railroad::Railroad (WareHouse* w1, WareHouse* w2) 
  : Building(w1->position)
  , Iterable<Railroad>(this)
  , oneEnd(w1->position)
  , twoEnd(w2->position)
  , oneHouse(w1)
  , twoHouse(w2)
  , currentLoad(0)
  , locosToBuild(0)
{
  calcEnds(); 
  w1->addRailroad(this);
  w2->addRailroad(this);
  toCompletion = getLength(); 
}

Factory::Factory (point p) 
  : Building(p)
  , Iterable<Factory>(this)
  , m_WareHouse(p)
{}

WareHouse::WareHouse (point p) 
  : Building(p)
  , Iterable<WareHouse>(this)
  , release(true)
  , content(0)
  , activeRail(0)
{
  toCompletion = newBuildSize;
  capacity = newBuildSize; 
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

void WareHouse::connect (WareHouse* other) {
  if (!other) return; 
  if (player != other->player) return;

  Railroad* existing = Railroad::findConnector(this, other);
  if (!existing) {
    existing = new Railroad(this, other);
    existing->player = player;
  }
  existing->upgrade(); 
}

double WareHouse::getCompFraction () const {
  if (0 >= toCompletion) return 1.0;
  double ret = newBuildSize; 
  ret = 1.0 - toCompletion / ret;
  return ret;
}

void WareHouse::receive (Packet* packet) {
  // Four options:
  // 1. Use towards completing this building.
  // 2. Send on outgoing railroad with capacity.
  // 3. Store.
  // 4. Release. 

  if (packet->player != player) {
    delete packet; // For you, the war is over.
    return; 
  }

  if (!useToBuild(packet)) return; 
  if ((activeRail) && (activeRail->canAccept(packet))) {
    activeRail->receive(packet, this);
    return; 
  }

  if ((release) || (capacity < content + packet->getSize())) {
    releaseTroops(packet->getSize());
  }
  else {
    content += packet->getSize();
  }
  delete packet; 
}

void Building::releaseTroops (int size, Tile* t) {
  if (!t) t = tile; 
  int numVertices = 0; 
  for (Vertex::Iter v = t->startv(); v != t->finalv(); ++v) {
    if ((*v)->player != player) continue; 
    if (1 >= (*v)->getFrontDistance()) continue;
    numVertices++; 
  }
  if (0 == numVertices) return;
  size /= numVertices; 
  for (Vertex::Iter v = t->startv(); v != t->finalv(); ++v) {
    if ((*v)->player != player) continue; 
    if (1 >= (*v)->getFrontDistance()) continue;
    (*v)->troops += size; 
  }
}

void WareHouse::replaceRail (Railroad* oldRail, Railroad* newRail) {
  if (activeRail == oldRail) activeRail = newRail;
  bool found = false; 
  for (unsigned int i = 0; i < outgoing.size(); ++i) {
    if (outgoing[i] != oldRail) continue;
    outgoing[i] = newRail;
    found = true;
    break; 
  }
  if (!found) outgoing.push_back(newRail); 
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
  if (0.25 > tile->avgControl(player)) {
    content /= 2;
    release = true;
    player = !player;
    activeRail = 0; 
    toCompletion = newBuildSize;
  }

  if (0 == content) return;
  if (!release) return;
  releaseTroops(content);
  content = 0;
}

void Factory::produce (int elapsedTime) {
  timeSinceProduction += elapsedTime;
  if (timeSinceProduction < timeToProduce) return; 
  timeSinceProduction -= timeToProduce; 
  
  Packet* product = new Packet();
  product->size = capacity;
  product->position = position;
  product->player = player; 
  m_WareHouse.receive(product); 
}

void Railroad::calcEnds () {
  oneEnd = oneHouse->position;
  twoEnd = twoHouse->position;
  point line = twoEnd - oneEnd;
  line.normalise(); 
  ortho.x() = -(line.y());
  ortho.y() = line.x(); 
  line *= 15;
  oneEnd = oneHouse->position + line;
  twoEnd = twoHouse->position - line;
  
}

bool Railroad::canAccept (Packet* packet) {
  if (capacity > currentLoad) return true;
  if (0 < locosToBuild) return true;
  return false; 
}

double Railroad::getCompFraction () const {
  if (0 >= toCompletion) return 1.0;
  double ret = getLength();
  ret = 1.0 - toCompletion / ret;
  return ret;
}

int Railroad::getLength () const {
  point line = twoEnd;
  line -= oneEnd;
  return (int) floor(line.length() + 0.5); 
}

Railroad* Railroad::findConnector (WareHouse* w1, WareHouse* w2) {
  for (Iter r = start(); r != final(); ++r) {
    if (((*r)->oneHouse == w1) && ((*r)->twoHouse == w2)) return (*r);
    if (((*r)->oneHouse == w2) && ((*r)->twoHouse == w1)) return (*r);
  }
  return 0; 
}

void Railroad::receive (Packet* packet, WareHouse* source) {
  if (!useToBuild(packet)) return; 
  if (0 < locosToBuild) {
    locosToBuild--;
    delete packet; 
    return; 
  }
  packet->target = (source == oneHouse ? twoHouse : oneHouse); 
  packets.push_back(packet); 
  currentLoad++; 
}

void Railroad::update (int elapsedTime) {
  vector<Packet*> removes;
  assert(packets.size() <= capacity); 
  for (unsigned int p = 0; p < packets.size(); ++p) {
    packets[p]->tile = Tile::getClosest(packets[p]->position, packets[p]->tile);
    if (1 >= packets[p]->tile->frontDistance()) {
      currentLoad--; 
      releaseTroops(packets[p]->size, packets[p]->tile); 
      delete packets[p];
      packets[p] = 0;
      continue; 
    }

    point direction = (packets[p]->target->position - packets[p]->position);
    double distance = direction.length();
    if (distance < speed*elapsedTime) {
      packets[p]->target->receive(packets[p]);
      currentLoad--; 
      // No delete here, we're passing the packet on. 
      packets[p] = 0; 
      continue;
    }
    direction.normalise(); 
    direction *= (speed*elapsedTime);
    packets[p]->position += direction;
  }

  for (unsigned int p = 0; p < packets.size(); ++p) {
    if (packets[p]) continue;
    packets[p] = packets.back(); 
    packets.pop_back(); 
  }
}

void Railroad::upgrade () {
  capacity++; 
  locosToBuild++; 
}
