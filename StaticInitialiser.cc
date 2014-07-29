#include "StaticInitialiser.hh"
#include "Factory.hh"
#include "Graphics.hh" 
#include "Parser.hh"
#include "Tile.hh" 

const double displayToInternal = 1e-6;

void StaticInitialiser::createFactory (Object* fact) {
  point location(fact->safeGetFloat("xpos"), fact->safeGetFloat("ypos"));
  Factory* fac = new Factory(location);
  fac->player = (fact->safeGetString("human", "no") == "yes");
  fac->m_Throughput = fact->safeGetFloat("throughput") * displayToInternal; // Per second in the file, per microsecond internally. 
  fac->m_WareHouse.player = fac->player;
  fac->m_WareHouse.m_ai->setFactory(fac);
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    fac->m_WareHouse.m_Structure[**i] = fac->m_WareHouse.getStructureAmount(*i);
  }
  Object* rawMaterials = fact->safeGetObject("rawMaterials");
  new FactoryGraphics(fac);
  if (rawMaterials) {
    RawMaterialProducer* rmp = new RawMaterialProducer(&(fac->m_WareHouse));
    rmp->player = fac->player; 
    createRawMaterialProducer(rawMaterials, rmp);
    new ProducerGraphics(rmp);
    fac->m_WareHouse.m_ai->setRmp(rmp); 
  }
  new WareHouseGraphics(&(fac->m_WareHouse)); 
}

void StaticInitialiser::createRawMaterialProducer (Object* def, RawMaterialProducer* rmp) {
  Object* maxProd = def->getNeededObject("maxProduction");
  Object* curProd = def->getNeededObject("currProduction");

  double totalCurr = 0;
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    double prod = maxProd->safeGetFloat((*i)->getName());
    prod *= displayToInternal; // Per second in file, per microsecond internally. 
    if (prod < 0.001) prod = 0.001;
    rmp->maxProduction.add(*i, prod);
    prod = curProd->safeGetFloat((*i)->getName()); 
    if (prod < 0) prod = 0;
    rmp->curProduction.add(*i, prod);
    totalCurr += prod;
  }

  // Enforce nonzero current production
  if (totalCurr < 1e-6) {
    rmp->curProduction.add(0, 1 - totalCurr);
    totalCurr = 1;
  }

  // Normalise current production to 1
  rmp->curProduction.normalise();  
}

void StaticInitialiser::initialise () {
  Object* config = processFile("config.txt"); 
  Railroad::speed = config->safeGetFloat("packetSpeed", Railroad::speed); 
  Vertex::troopMoveRate = config->safeGetFloat("troopMoveRate", Vertex::troopMoveRate);
  Vertex::fightRate = config->safeGetFloat("fightRate", Vertex::fightRate); 
  Vertex::minimumGarrison = config->safeGetFloat("minimumGarrison", Vertex::minimumGarrison); 
  Vertex::coolDownFactor = config->safeGetFloat("cooldown", Vertex::coolDownFactor); 
  Vertex::attritionRate = config->safeGetFloat("attrition", Vertex::attritionRate);
  Vertex::artilleryEffect = config->safeGetFloat("artilleryEffect", Vertex::artilleryEffect);
  Vertex::aircraftArtilleryBonus = config->safeGetFloat("aircraftArtilleryBonus", Vertex::aircraftArtilleryBonus);
  Vertex::aircraftInfantryBonus = config->safeGetFloat("aircraftInfantryBonus", Vertex::aircraftInfantryBonus);
  WareHouse::s_ArtilleryRangeSq = pow(config->safeGetFloat("artilleryRange", sqrt(WareHouse::s_ArtilleryRangeSq)), 2);
  WareHouse::s_AircraftRangeSq = pow(config->safeGetFloat("aircraftRange", sqrt(WareHouse::s_AircraftRangeSq)), 2);
  Object* sortieCost = config->safeGetObject("sortieCost");
  if (sortieCost) loadRawMaterials(sortieCost, &(WareHouse::s_SortieCost));
  else {
    WareHouse::s_SortieCost[RawMaterial::Fuel] = 1;
    WareHouse::s_SortieCost[RawMaterial::Ammo] = 1;
  }

  Object* ai = config->getNeededObject("ai");
  WarehouseAI::defcon5 = ai->safeGetInt("defcon5", WarehouseAI::defcon5);
  WarehouseAI::defcon4 = ai->safeGetInt("defcon4", WarehouseAI::defcon4);
  WarehouseAI::defcon3 = ai->safeGetInt("defcon3", WarehouseAI::defcon3);
  WarehouseAI::defcon2 = ai->safeGetInt("defcon2", WarehouseAI::defcon2);

  Locomotive::decayRate = (-1.0) / config->safeGetFloat("locoDecayDistance", Locomotive::decayRate);
  Locomotive::repairRate = (-1.0) / config->safeGetFloat("locoRepairTime", Locomotive::repairRate);

  for (UnitType::Iter ut = UnitType::start(); ut != UnitType::final(); ++ut) {
    string keyword = (*ut)->getName();
    keyword += "Cost";
    Object* currCost = config->safeGetObject(keyword);
    assert(currCost);
    loadRawMaterials(currCost, &Factory::s_ProductionCosts[**ut]);
  }

  Object* railCost = config->safeGetObject("railCost");
  assert(railCost);
  loadRawMaterials(railCost, &Railroad::s_Structure);

  railCost = config->safeGetObject("baseCost");
  assert(railCost);
  loadRawMaterials(railCost, &WareHouse::s_Structure);

  Object* defaultUnits = config->safeGetObject("defaultUnitStructure");
  loadUnits(defaultUnits, &WareHouse::s_DefaultUnitsDesired); 
}

void StaticInitialiser::loadSave (string fname) {
  Object* scenario = processFile(fname);
  objvec facts = scenario->getValue("factory"); 
  for (objiter fact = facts.begin(); fact != facts.end(); ++fact) {
    createFactory(*fact); 
  }
}

void StaticInitialiser::loadRawMaterials (Object* def, RawMaterialHolder* rmh) {
  rmh->clear();
  if (!def) return; 
  for (RawMaterial::Iter i = RawMaterial::start(); i != RawMaterial::final(); ++i) {
    rmh->add((*i), def->safeGetFloat((*i)->getName()));
  }
}

void StaticInitialiser::loadUnits (Object* def, UnitHolder* uh) {
  uh->clear();
  if (!def) return; 
  for (UnitType::Iter u = UnitType::start(); u != UnitType::final(); ++u) {
    (*uh)[*u] = def->safeGetInt((*u)->getName()); 
  }
}
