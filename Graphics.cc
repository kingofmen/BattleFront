#include "Graphics.hh"

FactoryGraphics::FactoryGraphics (Factory* f)
  : Iterable<FactoryGraphics>(this)
  , m_Factory(f)
{
  m_WareHouseDrawer = new WareHouseGraphics(&(m_Factory->m_WareHouse)); 
}

FactoryGraphics::~FactoryGraphics () {}

WareHouseGraphics::WareHouseGraphics (WareHouse* w)
  : Iterable<WareHouseGraphics>(this)
  , m_WareHouse(w)
{}

WareHouseGraphics::~WareHouseGraphics () {}


void FactoryGraphics::draw () {
  if (m_Factory->player) glColor3d(0.0, 0.0, 1.0);
  else glColor3d(1.0, 0.0, 0.0);
  
  glVertex2d(m_Factory->position.x() + 0.0, m_Factory->position.y() - 9.3 - 1.5);
  glVertex2d(m_Factory->position.x() - 8.0, m_Factory->position.y() + 6.7 - 1.5);
  glVertex2d(m_Factory->position.x() + 8.0, m_Factory->position.y() + 6.7 - 1.5);
}

void WareHouseGraphics::draw () {
  if (m_WareHouse->player) glColor3d(0.0, 0.0, 0.2 + 0.8*m_WareHouse->getCompFraction());
  else glColor3d(0.2 + 0.8*m_WareHouse->getCompFraction(), 0.0, 0.0);
  
  glVertex2d(m_WareHouse->position.x() + 0.0, m_WareHouse->position.y() + 9.3 + 1.5);
  glVertex2d(m_WareHouse->position.x() + 8.0, m_WareHouse->position.y() - 6.7 + 1.5);
  glVertex2d(m_WareHouse->position.x() - 8.0, m_WareHouse->position.y() - 6.7 + 1.5);
}
