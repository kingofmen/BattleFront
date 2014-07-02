#include "Adapters.hh"
#include "Factory.hh" 
#include "Graphics.hh"
#include "Button.hh" 

Button** ProducerButtonAdapter::increase = 0;
Button** ProducerButtonAdapter::decrease = 0;
ProducerButtonAdapter* ProducerButtonAdapter::instance = 0; 

ProducerButtonAdapter* ProducerButtonAdapter::getInstance () {
  if (instance) return instance;
  instance = new ProducerButtonAdapter();
  increase = new Button*[RawMaterial::NumRawMaterials];
  decrease = new Button*[RawMaterial::NumRawMaterials];
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    increase[**r] = new Button(50, 5 + (**r)*12, 32, 32);
    increase[**r]->registerListener(instance);
    decrease[**r] = new Button(10, 5 + (**r)*12, 32, 32);
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
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    increase[**r]->setActive(act);
    decrease[**r]->setActive(act);
  }
}
