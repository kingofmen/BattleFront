#include "StaticInitialiser.hh"
#include "Factory.hh"
#include "Parser.hh"
#include "Tile.hh" 

void StaticInitialiser::createFactory (Object* fact) {
  Factory* fac = new Factory(point(fact->safeGetFloat("xpos"), fact->safeGetFloat("ypos")));
  fac->player = (fact->safeGetString("human", "no") == "yes");
  fac->timeToProduce = fact->safeGetInt("timeToProduce"); 
  fac->timeSinceProduction = 0;
  setPacket(fact->safeGetObject("production"), &(fac->production));
  fac->m_WareHouse.player = fac->player;
  fac->m_WareHouse.toCompletion = 0;
  Object* rawMaterials = fact->safeGetObject("rawMaterials");
  if (rawMaterials) {
    RawMaterialProducer* rmp = new RawMaterialProducer(&(fac->m_WareHouse));
    createRawMaterialProducer(rawMaterials, rmp); 
  }
}

void StaticInitialiser::createRawMaterialProducer (Object* def, RawMaterialProducer* rmp) {
  Object* maxProd = def->getNeededObject("maxProduction");
  Object* curProd = def->getNeededObject("currProduction");

  double totalCurr = 0;
  for (unsigned int i = 0; i < NumRawMaterials; ++i) {
    double prod = maxProd->safeGetFloat(RawMaterialHolder::getName(i));
    if (prod < 0.001) prod = 0.001;
    rmp->maxProduction.add(i, prod);
    prod = curProd->safeGetFloat(RawMaterialHolder::getName(i));
    if (prod < 0) prod = 0;
    rmp->curProduction.add(i, prod);
    totalCurr += prod;
  }

  // Enforce nonzero current production
  if (totalCurr < 1e-6) {
    rmp->curProduction.add(0, 1 - totalCurr);
    totalCurr = 1;
  }

  // Normalise current production to 1
  totalCurr = 1.0 / totalCurr;
  for (unsigned int i = 0; i < NumRawMaterials; ++i) {
    double curr = rmp->curProduction.get(i);
    rmp->curProduction.add(i, -curr);
    curr *= totalCurr;
    rmp->curProduction.add(i, curr);
  }
  
}

void StaticInitialiser::initialise () {
  Object* config = processFile("config.txt"); 
  Railroad::speed = config->safeGetFloat("packetSpeed", Railroad::speed); 
  Vertex::troopMoveRate = config->safeGetFloat("troopMoveRate", Vertex::troopMoveRate);
  Vertex::fightRate = config->safeGetFloat("fightRate", Vertex::fightRate); 
  Vertex::minimumGarrison = config->safeGetFloat("minimumGarrison", Vertex::minimumGarrison); 
  Vertex::coolDownFactor = config->safeGetFloat("cooldown", Vertex::coolDownFactor); 
  Vertex::attritionRate = config->safeGetFloat("attrition", Vertex::attritionRate);
  WareHouse::newBuildSize = config->safeGetInt("newDepotSize", WareHouse::newBuildSize);

  Object* ai = config->getNeededObject("ai");
  WarehouseAI::defcon5 = ai->safeGetInt("defcon5", WarehouseAI::defcon5);
  WarehouseAI::defcon4 = ai->safeGetInt("defcon4", WarehouseAI::defcon4);
  WarehouseAI::defcon3 = ai->safeGetInt("defcon3", WarehouseAI::defcon3);
  WarehouseAI::defcon2 = ai->safeGetInt("defcon2", WarehouseAI::defcon2);

  setPacket(config->safeGetObject("locomotiveCost"), &(Factory::locomotiveCost));
  setPacket(config->safeGetObject("regimentalCost"), &(Factory::regimentalCost));
  Locomotive::decayRate = (-1.0) / config->safeGetFloat("locoDecayDistance", Locomotive::decayRate);
  Locomotive::repairRate = (-1.0) / config->safeGetFloat("locoRepairTime", Locomotive::repairRate);
}

void StaticInitialiser::loadSave (string fname) {
  Object* scenario = processFile(fname);
  objvec facts = scenario->getValue("factory"); 
  for (objiter fact = facts.begin(); fact != facts.end(); ++fact) {
    createFactory(*fact); 
  }
}

void StaticInitialiser::setPacket (Object* object, Packet* packet) {
  if (!object) return;
  packet->clear(); 
  packet->add(Packet::Manpower, object->safeGetInt("manpower"));
  packet->add(Packet::Gasoline, object->safeGetInt("gasoline"));
  packet->add(Packet::Materiel, object->safeGetInt("materiel"));   
}
