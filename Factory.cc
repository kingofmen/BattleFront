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

WarehouseAI::WarehouseAI (WareHouse* w)
  : m_WareHouse(w)
  , reinforceTarget(0)
  , reinforcePercentage(0)
  , statusChanged(false)
  , connection(0) 
{}

WareHouse::WareHouse (point p) 
  : Building(p)
  , Iterable<WareHouse>(this)
  , release(Release)
  , content(0)
  , activeRail(0)
  , m_ai(new WarehouseAI(this))
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

Railroad* WareHouse::connect (WareHouse* other) {
  if (!other) return 0; 
  if (player != other->player) return 0;

  Railroad* existing = Railroad::findConnector(this, other);
  if (!existing) {
    existing = new Railroad(this, other);
    existing->player = player;
  }
  existing->upgrade();
  return existing; 
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
    m_ai->notify(packet->size, WarehouseAI::Passed);
    return; 
  }

  if ((Release == release) || (Hold == release) || (capacity < content + packet->getSize())) {
    releaseTroops(packet->getSize());
    m_ai->notify(packet->size, WarehouseAI::Released); 
  }
  else {
    content += packet->getSize();
    m_ai->notify(packet->size, WarehouseAI::Held); 
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

void WareHouse::toggleHoldState () {
  switch (release) {
  default:
  case Release:    release = Accumulate; break;
  case Accumulate: release = Hold;       break;
  case Hold:       release = Release;    break; 
  }
}

void WareHouse::toggleRail () {
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

void WareHouse::update (int elapsedTime) {
  if (0.25 > tile->avgControl(player)) {
    content /= 2;
    release = Release;
    player = !player;
    activeRail = 0; 
    toCompletion = newBuildSize;
  }

  if (!player) m_ai->update(elapsedTime); 
  if ((0 < content) && (Release == release)) {
    releaseTroops(content);
    content = 0;
  } 
}

void Factory::produce (int elapsedTime) {
  timeSinceProduction += elapsedTime;
  if (timeSinceProduction < timeToProduce) return;
  Packet* product = new Packet();
  
  while (timeSinceProduction > timeToProduce) {
    timeSinceProduction -= timeToProduce; 
    product->size += capacity;
  }
  
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

void WarehouseAI::globalAI () {
  // Search for non-player warehouse that needs reinforcement.

  WareHouse* endangered = 0;
  for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) {
    if ((*w)->player) continue;

    int frontDistance = (*w)->tile->frontDistance();
    if      (frontDistance > 100) (*w)->m_ai->threatLevel = 0;
    else if (frontDistance > 75)  (*w)->m_ai->threatLevel = 1;
    else if (frontDistance > 50)  (*w)->m_ai->threatLevel = 2;
    else if (frontDistance > 25)  (*w)->m_ai->threatLevel = 3;
    else                          (*w)->m_ai->threatLevel = 4;

    if (0 == (*w)->m_ai->threatLevel) continue;
    if ((0 == endangered) || ((*w)->m_ai->threatLevel > endangered->m_ai->threatLevel)) endangered = (*w); 
  }

  if (!endangered) return; 

  endangered->m_ai->setReinforceTarget(0); 
  for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) {
    if ((*w)->player) continue;
    if ((*w) == endangered) continue;
    if ((*w)->m_ai->threatLevel == endangered->m_ai->threatLevel) continue; 

    (*w)->m_ai->setReinforceTarget(endangered);
    (*w)->m_ai->reinforcePercentage = 0.5; 
  }
}


void WarehouseAI::setReinforceTarget (WareHouse* t) {
  if (t == reinforceTarget) return; 
  reinforceTarget = t;
  if ((0 == reinforceTarget) || (m_WareHouse == reinforceTarget)) {
    m_WareHouse->activeRail = 0;
    return; 
  }

  connection = Railroad::findConnector(m_WareHouse, reinforceTarget);
  if (connection) return;

  // Look for WareHouse that is
  // a) closer to target than this and
  // b) closest to this

  WareHouse* intermediate = reinforceTarget;
  double maxDistSq = m_WareHouse->position.distanceSq(reinforceTarget->position);
  double leastDistSq = maxDistSq; 
  for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) {
    if ((*w)->player != m_WareHouse->player) continue;
    if ((*w) == m_WareHouse) continue;
    if ((*w) == reinforceTarget) continue; // Save a couple of distance calcs. 
    double curr = (*w)->position.distanceSq(reinforceTarget->position);
    if (curr > maxDistSq) continue;
    curr = (*w)->position.distanceSq(m_WareHouse->position);
    if (curr > leastDistSq) continue;
    leastDistSq = curr;
    intermediate = (*w); 
  }

  connection = Railroad::findConnector(m_WareHouse, intermediate);
  if (connection) return;
  connection = m_WareHouse->connect(intermediate);
  // That should always work, otherwise something is wrong.
  assert(connection); 
}

void WarehouseAI::notify (int size, Action act) {
  packets.push_back(pair<int, Action>(size, act));
  if (packets.size() > 50) packets.pop_front();
  statusChanged = true; 
}

void WarehouseAI::update (int elapsedTime) {
  if (!statusChanged) return;
  statusChanged = false; 

  if (!reinforceTarget) return;
  double total = 0;
  double reinforced = 0;
  for (list<pair<int, Action> >::iterator p = packets.begin(); p != packets.end(); ++p) {
    total += (*p).first;
    if (Passed == (*p).second) reinforced += (*p).first;
  }
  reinforced /= total;
  if (reinforced > reinforcePercentage) m_WareHouse->activeRail = 0;
  else m_WareHouse->activeRail = connection;
}
