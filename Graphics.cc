#include "Graphics.hh"

FactoryGraphics::FactoryGraphics (Factory* f)
  : Graphics<FactoryGraphics, Factory>(this, f)
  , m_WareHouseDrawer(0)
  , m_ProducerDrawer(0)
{
  m_WareHouseDrawer = new WareHouseGraphics(&(m_Drawable->m_WareHouse)); 
}

WareHouseGraphics::WareHouseGraphics (WareHouse* w)
  : Graphics<WareHouseGraphics, WareHouse>(this, w)
{}

ProducerGraphics::ProducerGraphics (RawMaterialProducer* p)
  : Graphics<ProducerGraphics, RawMaterialProducer>(this, p)
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
