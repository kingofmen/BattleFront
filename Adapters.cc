#include "Adapters.hh"
#include "Factory.hh" 
#include "Graphics.hh"
#include "Button.hh" 

Button** ProducerButtonAdapter::s_Increase = 0;
Button** ProducerButtonAdapter::s_Decrease = 0;
ProducerButtonAdapter* ProducerButtonAdapter::instance = 0;
Button** FactoryButtonAdapter::orderUnits = 0;
FactoryButtonAdapter* FactoryButtonAdapter::instance = 0; 
Button** WarehouseButtonAdapter::s_IncreaseUnits = 0;
Button** WarehouseButtonAdapter::s_DecreaseUnits = 0;
WarehouseButtonAdapter* WarehouseButtonAdapter::instance = 0; 
Button* WarehouseButtonAdapter::s_IncArtillery = 0;
Button* WarehouseButtonAdapter::s_DecArtillery = 0;
Button* WarehouseButtonAdapter::s_IncAircraft = 0;
Button* WarehouseButtonAdapter::s_DecAircraft = 0;


ProducerButtonAdapter* ProducerButtonAdapter::getInstance () {
  if (instance) return instance;
  instance = new ProducerButtonAdapter();
  s_Increase = new Button*[RawMaterial::NumRawMaterials];
  s_Decrease = new Button*[RawMaterial::NumRawMaterials];
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    s_Increase[**r] = new Button(120, 15 + (**r)*25, 20, 20);
    s_Increase[**r]->registerListener(instance);
    s_Decrease[**r] = new Button(10, 15 + (**r)*25, 20, 20);
    s_Decrease[**r]->registerListener(instance);

    new ButtonGraphics(s_Increase[**r]);
    new ButtonGraphics(s_Decrease[**r]);
  }
}

void ProducerButtonAdapter::clicked (unsigned int buttonId) {
  RawMaterialProducer* rmp = ProducerGraphics::getSelected();
  // Null rmp should never happen, since the buttons are only active when something is selected.
  assert(rmp);
  // But if it does, fail gracefully, if silently.
  if (!rmp) return; 
  
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    if      (buttonId == s_Increase[**r]->getIdx()) {rmp->increaseProduction(*r); break;}
    else if (buttonId == s_Decrease[**r]->getIdx()) {rmp->decreaseProduction(*r); break;}
  }
  
}

void ProducerButtonAdapter::setActive (bool act) {
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    s_Increase[**r]->setActive(act);
    s_Decrease[**r]->setActive(act);
  }
}

FactoryButtonAdapter* FactoryButtonAdapter::getInstance () {
  if (instance) return instance;
  instance = new FactoryButtonAdapter();
  orderUnits = new Button*[UnitType::NumUnitTypes];
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    orderUnits[**r] = new Button(10, 230 + (**r)*25, 20, 20);
    orderUnits[**r]->registerListener(instance);

    new ButtonGraphics(orderUnits[**r]);
  }
}

void FactoryButtonAdapter::clicked (unsigned int buttonId) {
  Factory* fac = FactoryGraphics::getSelected();
  // Null fac should never happen, since the buttons are only active when something is selected.
  assert(fac);
  // But if it does, fail gracefully, if silently.
  if (!fac) return; 
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    if (buttonId != orderUnits[**r]->getIdx()) continue;
    fac->orderUnit(*r); 
    break; 
  }
  
}

void FactoryButtonAdapter::setActive (bool act) {
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    orderUnits[**r]->setActive(act);
  }
}

WarehouseButtonAdapter* WarehouseButtonAdapter::getInstance () {
  if (instance) return instance;
  instance = new WarehouseButtonAdapter();
  s_IncreaseUnits = new Button*[UnitType::NumUnitTypes];
  s_DecreaseUnits = new Button*[UnitType::NumUnitTypes];
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    s_IncreaseUnits[**r] = new Button(165, 130 + (**r)*25, 20, 20);
    s_IncreaseUnits[**r]->registerListener(instance);
    new ButtonGraphics(s_IncreaseUnits[**r]);

    s_DecreaseUnits[**r] = new Button(110, 130 + (**r)*25, 20, 20);
    s_DecreaseUnits[**r]->registerListener(instance);
    new ButtonGraphics(s_DecreaseUnits[**r]);    
  }

  s_IncArtillery = new Button(255, 130, 20, 20);
  s_DecArtillery = new Button(200, 130, 20, 20);
  s_IncAircraft = new Button(255, 155, 20, 20);
  s_DecAircraft = new Button(200, 155, 20, 20);

  new ButtonGraphics(s_IncArtillery);
  new ButtonGraphics(s_DecArtillery);  
  new ButtonGraphics(s_IncAircraft);
  new ButtonGraphics(s_DecAircraft);  

  s_IncArtillery->registerListener(instance);
  s_DecArtillery->registerListener(instance);
  s_IncAircraft->registerListener(instance);
  s_DecAircraft->registerListener(instance);

  
}

void WarehouseButtonAdapter::clicked (unsigned int buttonId) {
  WareHouse* fac = WareHouseGraphics::getSelected();
  // Null fac should never happen, since the buttons are only active when something is selected.
  assert(fac);
  // But if it does, fail gracefully, if silently.
  if (!fac) return; 
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    if (buttonId == s_IncreaseUnits[**r]->getIdx()) {
      fac->desire(*r, 1);
      return;
    }
    else if (buttonId == s_DecreaseUnits[**r]->getIdx()) {
      fac->desire(*r, -1);
      return; 
    }
  }

  if (buttonId == s_IncArtillery->getIdx()) {
    fac->changePace(UnitType::getArtillery(), 1);
    return;
  }
  if (buttonId == s_DecArtillery->getIdx()) {
    fac->changePace(UnitType::getArtillery(), -1);
    return;
  }
  if (buttonId == s_IncAircraft->getIdx()) {
    fac->changePace(UnitType::getAircraft(), 1);
    return;
  }
  if (buttonId == s_DecAircraft->getIdx()) {
    fac->changePace(UnitType::getAircraft(), -1);
    return;
  }
  
}

void WarehouseButtonAdapter::setActive (bool act) {
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    s_IncreaseUnits[**r]->setActive(act);
    s_DecreaseUnits[**r]->setActive(act);
  }
  s_IncArtillery->setActive(act);
  s_DecArtillery->setActive(act);
  s_IncAircraft->setActive(act);
  s_DecAircraft->setActive(act);

}

