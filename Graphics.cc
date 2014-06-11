#include "Graphics.hh"

FactoryGraphics::FactoryGraphics (Factory* f)
  : Graphics<FactoryGraphics, Factory>(this, f)
{}

WareHouseGraphics::WareHouseGraphics (WareHouse* w)
  : Graphics<WareHouseGraphics, WareHouse>(this, w)
{}

ProducerGraphics::ProducerGraphics (RawMaterialProducer* rmp)
  : Graphics<ProducerGraphics, RawMaterialProducer>(this, rmp)
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

  glBegin(GL_TRIANGLES);  
  glVertex2d(m_Drawable->position.x() + 0.0, m_Drawable->position.y() + 9.3 + 1.5);
  glVertex2d(m_Drawable->position.x() + 8.0, m_Drawable->position.y() - 6.7 + 1.5);
  glVertex2d(m_Drawable->position.x() - 8.0, m_Drawable->position.y() - 6.7 + 1.5);
  glEnd(); 
  
  if (m_Drawable == selected) {
    glColor3d(1.0, 1.0, 1.0); 
    glBegin(GL_LINE_STRIP);
    glVertex2d(m_Drawable->position.x() - 11, m_Drawable->position.y() - 11);
    glVertex2d(m_Drawable->position.x() + 13, m_Drawable->position.y() - 11);
    glVertex2d(m_Drawable->position.x() + 13, m_Drawable->position.y() + 13);
    glVertex2d(m_Drawable->position.x() - 11, m_Drawable->position.y() + 13);
    glVertex2d(m_Drawable->position.x() - 11, m_Drawable->position.y() - 11);
    glEnd();    
  }
}

void ProducerGraphics::draw () const {
}
