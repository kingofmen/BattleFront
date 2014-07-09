#include "Factory.hh"
#include "Tile.hh"
#include <cmath> 
#include <cassert> 

double Locomotive::decayRate = 1000; // Initially in pixels, will be inverted by StaticInitialiser
double Locomotive::repairRate = 1e6; // Initially in microseconds. 
double Railroad::speed = 0.0001;
int WareHouse::newBuildSize = 500; 
int WarehouseAI::defcon5 = 30;
int WarehouseAI::defcon4 = 25;
int WarehouseAI::defcon3 = 20;
int WarehouseAI::defcon2 = 10; 

UnitType* UnitType::UnitType1 = new UnitType("regiment", "Regiment", Regiment);
UnitType* UnitType::UnitType2 = new UnitType("locomotive", "Locomotive", Train);
UnitType* UnitType::UnitType3 = new UnitType("artillery", "Battery", Battery);
UnitType* UnitType::UnitType4 = new UnitType("aircraft", "Squadron", Squadron, true);

RawMaterial* RawMaterial::RawMaterial1 = new RawMaterial("men", Men);
RawMaterial* RawMaterial::RawMaterial2 = new RawMaterial("steel", Steel);
RawMaterial* RawMaterial::RawMaterial3 = new RawMaterial("fuel", Fuel);
RawMaterial* RawMaterial::RawMaterial4 = new RawMaterial("ammo", Ammo);

vector<RawMaterialHolder> Factory::s_ProductionCosts(UnitType::NumUnitTypes);

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
  , unableToProgress(0)
{
  setCurrentProduction();

}

Locomotive::Locomotive (WareHouse* h)
  : Iterable<Locomotive>(this)
  , maintenance(1.0)
  , home(h)
  , destination(0)
  , load(0)
{}

Locomotive::~Locomotive () {
  if (load) delete load; 
}

WarehouseAI::WarehouseAI (WareHouse* w)
  : m_WareHouse(w)
  , reinforceTarget(0)
  , reinforcePercentage(0)
  , statusChanged(false)
  , connection(0) 
{}

WareHouse::WareHouse (point p) 
  : Building(p)
  , RawMaterialHolder()
  , Iterable<WareHouse>(this)
  , release(Release)
  , content(0)
  , activeRail(0)
  , m_ai(new WarehouseAI(this))
{
  toCompletion = newBuildSize;
  capacity = newBuildSize; 
}

bool Building::checkOwnership () {
  // Returns true if ownership changed. 
  if (0.25 <= tile->avgControl(player)) return false;
  player = !player;
  return true;
}

bool Building::useToBuild (Packet* packet) {
  // Return true if the packet survives. 
  if (0 <= toCompletion) {
    if (toCompletion > packet->getSize()) {
      toCompletion -= packet->getSize();
      delete packet;
      return false;
    }
    packet->add(Packet::Manpower, -toCompletion);
    toCompletion = 0; 
  }
  return true;
}

void Locomotive::repair (int elapsedTime) {
  double diff = 1.0 - maintenance;
  diff *= exp(elapsedTime * repairRate); 
  maintenance = 1.0 - diff; 
}

void Locomotive::traverse (double distance) {
  maintenance *= exp(distance*decayRate);
}

Railroad* WareHouse::connect (WareHouse* other) {
  if (!other) return 0; 
  if (player != other->player) return 0;

  Railroad* existing = Railroad::findConnector(this, other);
  if (!existing) {
    existing = new Railroad(this, other);
    existing->player = player;
  }
  return existing; 
}

double WareHouse::getCompFraction () const {
  if (0 >= toCompletion) return 1.0;
  double ret = newBuildSize; 
  ret = 1.0 - toCompletion / ret;
  return ret;
}

void WareHouse::receive (Locomotive* loco, Railroad* source) {
  loco->position = position;
  loco->tile = tile; 
  if (loco->load) {
    receive(loco->load);
    loco->load = 0;
  }
  (*this) += (*loco);
  loco->clear(); 
  if (loco->home == this) locos.push_back(loco);
  else if (source) source->receive(loco, this);
  else {
    loco->home = this;
    locos.push_back(loco);
  }
}

void WareHouse::receive (UnitType* unit) {
  m_Units[unit]++; 
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
  if ((activeRail) && (0 < locos.size())) {
    Locomotive* loco = locos.front();
    locos.pop_front();
    loco->load = packet;
    activeRail->receive(loco, this);
    m_ai->notify(packet->getSize(), WarehouseAI::Passed);
    return; 
  }  

  if ((Release == release) || (Hold == release) || (capacity < content + packet->getSize())) {
    releaseTroops(packet->getSize());
    m_ai->notify(packet->getSize(), WarehouseAI::Released); 
  }
  else {
    content += packet->getSize();
    m_ai->notify(packet->getSize(), WarehouseAI::Held); 
  }
  delete packet; 
}

void WareHouse::sendLoco (WareHouse* other) {
  if (other == this) return;
  if (!other) return;
  if (0 == locos.size()) return; 
  Railroad* connection = Railroad::findConnector(this, other);
  if (!connection) return;
  if (!connection->complete()) return; 
  Locomotive* loco = locos.front();
  locos.pop_front();
  loco->home = other;
  connection->receive(loco, this); 
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

void WareHouse::toggleHoldState (bool backwards) {
  switch (release) {
  default:
  case Release:    release = backwards ? Hold       : Accumulate; break;
  case Accumulate: release = backwards ? Release    : Hold;       break;
  case Hold:       release = backwards ? Accumulate : Release;    break; 
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
  if (checkOwnership()) {
    content /= 2;
    release = Release;
    activeRail = 0; 
    toCompletion = newBuildSize;
  }
  if (!player) m_ai->update(elapsedTime); 
  if ((0 < content) && (Release == release)) {
    releaseTroops(content);
    content = 0;
  }

  if ((0 < m_Units[UnitType::Regiment]) && (Release == release)) {
    releaseTroops(250*m_Units[UnitType::Regiment]);
    m_Units[UnitType::Regiment] = 0; 
  }
  
  for (list<Locomotive*>::iterator loc = locos.begin(); loc != locos.end(); ++loc) {
    (*loc)->repair(elapsedTime);
  }

  if (0 < locos.size()) {
    double loadCap = getLoadCapacity() * elapsedTime;
    for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
      //if (outflows[**i])
      if (activeRail) {
	double amountToLoad = min(loadCap, get(*i)); 
	locos.front()->add(*i, loadCap);
	loadCap -= amountToLoad;
	if (loadCap < 1e-6) break;
      }
    }
    if ((0.5 < locos.front()->getLoadWeight()) && (activeRail)) {
      activeRail->receive(locos.front(), this);
      locos.pop_front(); 
    }
  }
}

double Factory::getCompletion () const {
  double used = 0;
  double total = 1e-6;
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    used += m_UsedSoFar.get(*i);
    total += s_ProductionCosts[*m_ProductionQueue.front()].get(*i);
  }
  return used/total; 
}

void Factory::orderUnit (UnitType* u) {
  m_ProductionQueue.push_back(u); 
}

void Factory::orderLoco () {
  m_ProductionQueue.push_back(UnitType::getByIndex(UnitType::Train)); 
}

void Factory::setCurrentProduction () {
  if (m_ProductionQueue.empty()) m_ProductionQueue.push_back(UnitType::getByIndex(UnitType::Regiment));
  m_NormalisedCost = s_ProductionCosts[*m_ProductionQueue.front()];
  m_NormalisedCost.normalise();   
}

void Factory::doneProducing () {
  UnitType* newUnitType = m_ProductionQueue.front();
  m_ProductionQueue.pop_front(); 
  setCurrentProduction();
  if (m_ProductionQueue.front() == newUnitType) m_UsedSoFar -= s_ProductionCosts[*newUnitType];
  else m_UsedSoFar.clear();
  m_WareHouse.receive(newUnitType); 
  
  switch (*newUnitType) {
  case UnitType::Regiment:
    {
    Packet* product = new Packet();
    product->add(Packet::Manpower, 50);
    product->position = position;
    product->player = player; 
    m_WareHouse.receive(product);
    
    }
    break;
  case UnitType::Train:
    m_WareHouse.receive(new Locomotive(&m_WareHouse), 0);
    break;
  case UnitType::Battery:
    break;
  case UnitType::Squadron:
    break; 
  default:
    break; 
  }
}

void Factory::produce (int elapsedTime) {
  if (checkOwnership()) {
    m_ProductionQueue.clear();
    setCurrentProduction();
    m_UsedSoFar.clear();
  }
  double currThroughput = elapsedTime * m_Throughput;
  RawMaterialHolder wantToUse = m_NormalisedCost * currThroughput;

  if (m_WareHouse >= wantToUse) {
    unableToProgress = 0;
    m_UsedSoFar += wantToUse;
    m_WareHouse -= wantToUse;
    if (m_UsedSoFar >= s_ProductionCosts[*m_ProductionQueue.front()]) doneProducing();
  }
  else {
    unableToProgress += elapsedTime;
    if (unableToProgress < 1000000) return;
    m_ProductionQueue.pop_front();
    setCurrentProduction(); 
  }
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

void Railroad::receive (Locomotive* loco, WareHouse* source) {
  if ((0 < toCompletion) && (loco->load) && (!useToBuild(loco->load))) {
    loco->load = 0;
    source->receive(loco, this); 
    return; 
  }

  if (0 < toCompletion) {
    source->receive(loco, this);
    return; 
  }
  
  locos.push_back(loco);
  if (source == oneHouse) loco->destination = twoHouse;
  else loco->destination = oneHouse;
}


void Railroad::update (int elapsedTime) {
  for (list<Locomotive*>::iterator loc = locos.begin(); loc != locos.end(); ++loc) {
    (*loc)->tile = Tile::getClosest((*loc)->position, (*loc)->tile);
    if (1 >= (*loc)->tile->frontDistance()) {
      if ((*loc)->load) {
	releaseTroops((*loc)->load->getSize(), (*loc)->tile); 
	delete (*loc)->load;
	(*loc)->load = 0;
      }
      (*loc)->destination = (*loc)->home;
      (*loc)->maintenance *= 0.90; 
    }

    point direction = ((*loc)->destination->position - (*loc)->position);
    double distance = direction.length();
    double covered = speed*elapsedTime*(*loc)->getSpeedModifier();
    (*loc)->traverse(min(distance, covered));
    if (distance < covered) {
      (*loc)->destination->receive((*loc), this);
      loc = locos.erase(loc);
      --loc; 
      continue;
    }
    direction.normalise(); 
    direction *= covered; 
    (*loc)->position += direction;
  }
}

void WarehouseAI::globalAI () {
  // Search for non-player warehouse that needs reinforcement.

  WareHouse* endangered = 0;
  for (WareHouse::Iter w = WareHouse::start(); w != WareHouse::final(); ++w) {
    if ((*w)->player) continue;

    int frontDistance = (*w)->tile->frontDistance();
    if      (frontDistance > defcon5)  (*w)->m_ai->threatLevel = 0;
    else if (frontDistance > defcon4)  (*w)->m_ai->threatLevel = 1;
    else if (frontDistance > defcon3)  (*w)->m_ai->threatLevel = 2;
    else if (frontDistance > defcon2)  (*w)->m_ai->threatLevel = 3;
    else                               (*w)->m_ai->threatLevel = 4;

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

RawMaterialHolder::RawMaterialHolder () :
  stockpile(RawMaterial::NumRawMaterials)
{
  clear();
}

RawMaterialHolder::RawMaterialHolder (double m, double s, double f, double a) :
  stockpile(RawMaterial::NumRawMaterials)
{
  stockpile[RawMaterial::Men] = m;
  stockpile[RawMaterial::Steel] = s;
  stockpile[RawMaterial::Fuel] = f;
  stockpile[RawMaterial::Ammo] = a; 
}

RawMaterialHolder::~RawMaterialHolder () {
}

RawMaterialProducer::RawMaterialProducer (WareHouse* w)
  : Building(w->position)
  , Iterable<RawMaterialProducer>(this)
  , maxProduction()
  , curProduction()
  , m_WareHouse(w)
{
  assert(w); 
}

RawMaterialProducer::~RawMaterialProducer () {}

void RawMaterialProducer::produce (int elapsedTime) {
  checkOwnership(); 
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) { 
    m_WareHouse->add(*i, maxProduction.get(*i) * curProduction.get(*i) * elapsedTime); 
  }
}

void RawMaterialProducer::increaseProduction (RawMaterial* rm) {
  double newProduction = min(1.0, curProduction.get(rm) + 0.01);
  setProduction(rm, newProduction);
}

void RawMaterialProducer::decreaseProduction (RawMaterial* rm) {
  double newProduction = max(0.0, curProduction.get(rm) - 0.01);
  setProduction(rm, newProduction);
}

void RawMaterialProducer::setProduction (RawMaterial* rm, double prod) {
  if (prod >= 1) {
    curProduction.clear();
    curProduction[*rm] = prod;
    return; 
  }

  double toRedist = curProduction[*rm] - prod;
  int numMaterials = RawMaterial::NumRawMaterials - 1;
  double fraction = 1.0 / numMaterials; 
  curProduction[*rm] = prod;
  while (fabs(toRedist) > 1e-6) {
    numMaterials = 0; 
    for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
      if ((*i) == rm) continue;
      if (curProduction[**i] + toRedist*fraction > 1) continue;
      if (curProduction[**i] + toRedist*fraction < 0) continue;
      numMaterials++;
      curProduction[**i] += toRedist*fraction;
    }
    assert(numMaterials > 0); 
    if (0 == numMaterials) break; // This should never happen.
    toRedist -= toRedist*numMaterials*fraction;
    fraction = 1.0 / numMaterials; 
  }
  
  curProduction.normalise(); // In case roundoff errors have crept in.
}

void RawMaterialHolder::clear () {
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) { 
    stockpile[**i] = 0;
  }
}

void RawMaterialHolder::normalise () {
  double total = 0;
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) total += stockpile[**i];
  if (0 == total) return;
  total = 1.0 / total;
  (*this) *= total; 
}

bool operator>= (const RawMaterialHolder& one, const RawMaterialHolder& two) {
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {  
    if (one.get(*i) < two.get(*i)) return false;
  }
  return true;
}

RawMaterialHolder operator* (const RawMaterialHolder& rmh, double num) {
  RawMaterialHolder ret;
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    ret.add(*i, rmh.get(*i) * num);
  }
  return ret; 
}

RawMaterialHolder operator* (double num, const RawMaterialHolder& rmh) {
  RawMaterialHolder ret;
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    ret.add(*i, rmh.get(*i) * num);
  }
  return ret; 
}

RawMaterialHolder& RawMaterialHolder::operator-= (const RawMaterialHolder& other) {
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    stockpile[**i] -= other.stockpile[**i]; 
  }
  return *this;
}

RawMaterialHolder& RawMaterialHolder::operator+= (const RawMaterialHolder& other) {
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    stockpile[**i] += other.stockpile[**i]; 
  }
  return *this;
}

RawMaterialHolder& RawMaterialHolder::operator*= (const double scale) {
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    stockpile[**i] *= scale;
  }
  return *this;
}
