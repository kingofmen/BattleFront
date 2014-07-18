#include "Adapters.hh"
#include "Factory.hh" 
#include "Graphics.hh"
#include "Button.hh" 

Button** ProducerButtonAdapter::increase = 0;
Button** ProducerButtonAdapter::decrease = 0;
ProducerButtonAdapter* ProducerButtonAdapter::instance = 0;
Button** FactoryButtonAdapter::orderUnits = 0;
FactoryButtonAdapter* FactoryButtonAdapter::instance = 0; 
Button** WarehouseButtonAdapter::increaseUnits = 0;
Button** WarehouseButtonAdapter::decreaseUnits = 0;
WarehouseButtonAdapter* WarehouseButtonAdapter::instance = 0; 


ProducerButtonAdapter* ProducerButtonAdapter::getInstance () {
  if (instance) return instance;
  instance = new ProducerButtonAdapter();
  increase = new Button*[RawMaterial::NumRawMaterials];
  decrease = new Button*[RawMaterial::NumRawMaterials];
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    increase[**r] = new Button(120, 15 + (**r)*25, 20, 20);
    increase[**r]->registerListener(instance);
    decrease[**r] = new Button(10, 15 + (**r)*25, 20, 20);
    decrease[**r]->registerListener(instance);

    new ButtonGraphics(increase[**r]);
    new ButtonGraphics(decrease[**r]);
  }
}

void ProducerButtonAdapter::clicked (unsigned int buttonId) {
  RawMaterialProducer* rmp = ProducerGraphics::getSelected();
  // Null rmp should never happen, since the buttons are only active when something is selected.
  assert(rmp);
  // But if it does, fail gracefully, if silently.
  if (!rmp) return; 
  
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    if      (buttonId == increase[**r]->getIdx()) {rmp->increaseProduction(*r); break;}
    else if (buttonId == decrease[**r]->getIdx()) {rmp->decreaseProduction(*r); break;}
  }
  
}

void ProducerButtonAdapter::setActive (bool act) {
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    increase[**r]->setActive(act);
    decrease[**r]->setActive(act);
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
  increaseUnits = new Button*[UnitType::NumUnitTypes];
  decreaseUnits = new Button*[UnitType::NumUnitTypes];
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    increaseUnits[**r] = new Button(165, 130 + (**r)*25, 20, 20);
    increaseUnits[**r]->registerListener(instance);
    new ButtonGraphics(increaseUnits[**r]);

    decreaseUnits[**r] = new Button(110, 130 + (**r)*25, 20, 20);
    decreaseUnits[**r]->registerListener(instance);
    new ButtonGraphics(decreaseUnits[**r]);    
  }
}

void WarehouseButtonAdapter::clicked (unsigned int buttonId) {
  WareHouse* fac = WareHouseGraphics::getSelected();
  // Null fac should never happen, since the buttons are only active when something is selected.
  assert(fac);
  // But if it does, fail gracefully, if silently.
  if (!fac) return; 
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    if (buttonId == increaseUnits[**r]->getIdx()) {
      fac->desire(*r, 1);
      break;
    }
    else if (buttonId == decreaseUnits[**r]->getIdx()) {
      fac->desire(*r, -1);
      break;  
    }
  }
  
}

void WarehouseButtonAdapter::setActive (bool act) {
  for (UnitType::Iter r = UnitType::start(); r != UnitType::final(); ++r) {
    increaseUnits[**r]->setActive(act);
    decreaseUnits[**r]->setActive(act);
  }
}

