#include "Graphics.hh"

FactoryGraphics::FactoryGraphics (Factory* f, point p)
  : Graphics<FactoryGraphics, Factory>(this, f, p)
{}

WareHouseGraphics::WareHouseGraphics (WareHouse* w, point p)
  : Graphics<WareHouseGraphics, WareHouse>(this, w, p)
{}

ProducerGraphics::ProducerGraphics (RawMaterialProducer* rmp, point p)
  : Graphics<ProducerGraphics, RawMaterialProducer>(this, rmp, p)
{}

FactoryGraphics::~FactoryGraphics () {}
WareHouseGraphics::~WareHouseGraphics () {}
ProducerGraphics::~ProducerGraphics () {}

void FactoryGraphics::draw () const {
  if (m_Drawable->player) glColor3d(0.0, 0.0, 1.0);
  else glColor3d(1.0, 0.0, 0.0);
  
  glVertex2d(m_Drawable->position.x() + 0.0, m_Drawable->position.y() - 9.3 - 1.5);
  glVertex2d(m_Drawable->position.x() - 8.0, m_Drawable->position.y() + 6.7 - 1.5);
  glVertex2d(m_Drawable->position.x() + 8.0, m_Drawable->position.y() + 6.7 - 1.5);
}

void WareHouseGraphics::draw () const {
  if (m_Drawable->player) glColor3d(0.0, 0.0, 0.2 + 0.8*m_Drawable->getCompFraction());
  else glColor3d(0.2 + 0.8*m_Drawable->getCompFraction(), 0.0, 0.0);
  
  glVertex2d(m_Drawable->position.x() + 0.0, m_Drawable->position.y() + 9.3 + 1.5);
  glVertex2d(m_Drawable->position.x() + 8.0, m_Drawable->position.y() - 6.7 + 1.5);
  glVertex2d(m_Drawable->position.x() - 8.0, m_Drawable->position.y() - 6.7 + 1.5);
}

void ProducerGraphics::draw () const {
}
