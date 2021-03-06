#include "Graphics.hh"

const double internalToDisplay = 1e6;

bool WareHouseGraphics::s_Overlays[WareHouseGraphics::NumOverlays]; 

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
  int xpos = 34;
  int offset = 5;
  int ypos = 234;
  for (deque<UnitType const*>::iterator u = m_Drawable->m_ProductionQueue.begin(); u != m_Drawable->m_ProductionQueue.end(); ++u) {
    offset += smallLetters->getWidth(smallLetters->renderText((*u)->getDisplayName(), xpos, ypos));
    if (u == m_Drawable->m_ProductionQueue.begin()) {
      offset += smallLetters->renderFloat(m_Drawable->getCompletion()*100, 1, xpos + offset, ypos);
      offset += smallLetters->renderText("%", xpos + offset, ypos);
    }
    ypos += 12; 
  }
  smallLetters->renderText("Regiment", xpos, ypos);
}

void drawCircle (point center, double radius) {
  glBegin(GL_POINTS);
  for (double rad = 0; rad < 2*M_PI; rad += 0.01) {
    glVertex2d(center.x() + radius*sin(rad), center.y() + radius*cos(rad)); 
  }
  glEnd();
}


void WareHouseGraphics::draw () const {
  if (m_Drawable->player) glColor3d(0.0, 0.0, 0.2 + 0.8*m_Drawable->getCompFraction());
  else glColor3d(0.2 + 0.8*m_Drawable->getCompFraction(), 0.0, 0.0);

  glBegin(GL_TRIANGLES); {
    glVertex2d(m_Drawable->position.x() + 0.0, m_Drawable->position.y() + 9.3 + 1.5);
    glVertex2d(m_Drawable->position.x() + 8.0, m_Drawable->position.y() - 6.7 + 1.5);
    glVertex2d(m_Drawable->position.x() - 8.0, m_Drawable->position.y() - 6.7 + 1.5);
  } glEnd();

  glColor3d(1.0, 1.0, 1.0);  
  if (s_Overlays[Artillery]) {
    drawCircle(m_Drawable->position, sqrt(WareHouse::s_ArtilleryRangeSq));
  }
  if (s_Overlays[Aircraft]) {
    drawCircle(m_Drawable->position, sqrt(WareHouse::s_AircraftRangeSq));
  }  
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

  smallLetters->renderText("Stockpile", 40, 110); 
  int textYPos = 124;
  int textXPos = 34;   
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    smallLetters->renderFloat(m_Drawable->get(*r), 2, textXPos, textYPos);
    textYPos += 25; 
  }

  smallLetters->renderText("Units", 140, 110); 
  textYPos = 124;
  for (UnitType::Iter u = UnitType::start(); u != UnitType::final(); ++u) {
    textXPos = 134;									    
    textXPos += smallLetters->renderInt(m_Drawable->m_Units.get(*u), textXPos, textYPos);
    textXPos += smallLetters->getWidth(smallLetters->renderText(" / ", textXPos, textYPos));
    textXPos += smallLetters->renderInt(m_Drawable->m_UnitsDesired.get(*u), textXPos, textYPos);
    textYPos += 25; 
  }

  smallLetters->renderText("Pace", 230, 110); 
  textYPos = 124;
  textXPos = 224;									    
  textXPos += smallLetters->renderInt(m_Drawable->m_ArtilleryPace, textXPos, textYPos);
  textXPos += smallLetters->getWidth(smallLetters->renderText(" / ", textXPos, textYPos));
  textXPos += smallLetters->renderInt(WareHouse::s_MaxArtilleryPace, textXPos, textYPos);
  textYPos += 25;
  textXPos = 224;									    
  textXPos += smallLetters->renderInt(m_Drawable->m_AirforcePace, textXPos, textYPos);
  textXPos += smallLetters->getWidth(smallLetters->renderText(" / ", textXPos, textYPos));
  textXPos += smallLetters->renderInt(WareHouse::s_MaxAirforcePace, textXPos, textYPos);
}

void WareHouseGraphics::toggle (Overlay over) {
  s_Overlays[over] = !s_Overlays[over];
}

void ProducerGraphics::draw () const {
}

void ProducerGraphics::drawSpecial () const {
  smallLetters->renderText("Production", 40, 0); 
  int textYPos = 14; 
  for (RawMaterial::Iter r = RawMaterial::start(); r != RawMaterial::final(); ++r) {
    int textXPos = 34; 
    textXPos += smallLetters->renderFloat(m_Drawable->curProduction.get(*r), 2, textXPos, textYPos);
    textXPos += smallLetters->getWidth(smallLetters->renderText(" : ", textXPos, textYPos)); 
    smallLetters->renderFloat(m_Drawable->maxProduction.get(*r) * m_Drawable->curProduction.get(*r) * internalToDisplay, 1, textXPos, textYPos);
    textYPos += 25; 
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
