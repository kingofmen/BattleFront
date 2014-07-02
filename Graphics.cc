#include "Graphics.hh"

const double internalToDisplay = 1e6;

FactoryGraphics::FactoryGraphics (Factory* f)
  : Graphics<FactoryGraphics, Factory>(this, f)
{}

WareHouseGraphics::WareHouseGraphics (WareHouse* w)
  : Graphics<WareHouseGraphics, WareHouse>(this, w)
{}

ProducerGraphics::ProducerGraphics (RawMaterialProducer* rmp)
  : Graphics<ProducerGraphics, RawMaterialProducer>(this, rmp)
{}

ButtonGraphics::ButtonGraphics (Button* b)
  : Graphics<ButtonGraphics, Button>(this, b)
{}


FactoryGraphics::~FactoryGraphics () {}
WareHouseGraphics::~WareHouseGraphics () {}
ProducerGraphics::~ProducerGraphics () {}
ButtonGraphics::~ButtonGraphics () {}

void FactoryGraphics::draw () const {
  if (m_Drawable->player) glColor3d(0.0, 0.0, 1.0);
  else glColor3d(1.0, 0.0, 0.0);
  
  glVertex2d(m_Drawable->position.x() + 0.0, m_Drawable->position.y() - 9.3 - 1.5);
  glVertex2d(m_Drawable->position.x() - 8.0, m_Drawable->position.y() + 6.7 - 1.5);
  glVertex2d(m_Drawable->position.x() + 8.0, m_Drawable->position.y() + 6.7 - 1.5);
}

void FactoryGraphics::drawSpecial () const {
}

void WareHouseGraphics::draw () const {
  if (m_Drawable->player) glColor3d(0.0, 0.0, 0.2 + 0.8*m_Drawable->getCompFraction());
  else glColor3d(0.2 + 0.8*m_Drawable->getCompFraction(), 0.0, 0.0);

  glBegin(GL_TRIANGLES); {
    glVertex2d(m_Drawable->position.x() + 0.0, m_Drawable->position.y() + 9.3 + 1.5);
    glVertex2d(m_Drawable->position.x() + 8.0, m_Drawable->position.y() - 6.7 + 1.5);
    glVertex2d(m_Drawable->position.x() - 8.0, m_Drawable->position.y() - 6.7 + 1.5);
  } glEnd();   
}

void WareHouseGraphics::drawSpecial () const {
  glColor3d(1.0, 1.0, 1.0); 
  glBegin(GL_LINE_STRIP); {
    glVertex2d(m_Drawable->position.x() - 11, m_Drawable->position.y() - 11);
    glVertex2d(m_Drawable->position.x() + 13, m_Drawable->position.y() - 11);
    glVertex2d(m_Drawable->position.x() + 13, m_Drawable->position.y() + 13);
    glVertex2d(m_Drawable->position.x() - 11, m_Drawable->position.y() + 13);
    glVertex2d(m_Drawable->position.x() - 11, m_Drawable->position.y() - 11);
  } glEnd();    
}

void ProducerGraphics::draw () const {
}

void ProducerGraphics::drawSpecial () const {
  int textYPos = 10; 
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    int textXPos = 10; 
    textXPos += smallLetters->renderFloat(m_Drawable->curProduction.get(*r), 2, textXPos, textYPos);
    textXPos += smallLetters->getWidth(smallLetters->renderText(" : ", textXPos, textYPos)); 
    smallLetters->renderFloat(m_Drawable->maxProduction.get(*r) * m_Drawable->curProduction.get(*r) * internalToDisplay, 1, textXPos, textYPos);
    textYPos += 15; 
  }
}

void ButtonGraphics::draw () const {
  if (!m_Drawable->isActive()) return; 
  glColor3d(1.0, 1.0, 1.0); 
  glBegin(GL_LINE_STRIP); {
    glVertex2d(m_Drawable->xpos                    , m_Drawable->ypos);
    glVertex2d(m_Drawable->xpos + m_Drawable->width, m_Drawable->ypos);
    glVertex2d(m_Drawable->xpos + m_Drawable->width, m_Drawable->ypos + m_Drawable->height);
    glVertex2d(m_Drawable->xpos                    , m_Drawable->ypos + m_Drawable->height);
    glVertex2d(m_Drawable->xpos                    , m_Drawable->ypos);
  } glEnd();    
}

void ButtonGraphics::drawSpecial () const {}
