#include "utils.hh" 
#include <cmath> 

point::point () 
  : boost::tuple<double, double>(0, 0) 
{}

point::point (double x, double y)  
  : boost::tuple<double, double>(x, y) 
{}

double& point::x() {return boost::get<0>(*this);}
double& point::y() {return boost::get<1>(*this);}

double const& point::x() const {return boost::get<0>(*this);}
double const& point::y() const {return boost::get<1>(*this);}

double point::angle (const point& other) const {
  double dot = x() * other.x();
  dot       += y() * other.y(); 
  dot       /= length();
  dot       /= other.length(); 
  return acos(dot); 
}

double point::distanceSq (const point& other) const {
  return pow(x() - other.x(), 2) + pow(y() - other.y(), 2); 
}

double point::distance (const point& other) const {
  return sqrt(distanceSq(other)); 
}

double point::lengthSq () const {
  return pow(x(), 2) + pow(y(), 2);
}

double point::length () const {
  return sqrt(lengthSq()); 
}

void point::normalise () {
  (*this) /= length(); 
}

void point::rotate (double angle) {
  // Clockwise rotation; angle is in radians
  double newx = x() * cos(angle) + y() * sin(angle);
  double newy = y() * cos(angle) - x() * sin(angle);
  x() = newx;
  y() = newy; 
}

point& point::operator+= (const point& other) {
  x() += other.x();
  y() += other.y();
}

point& point::operator-= (const point& other) {
  x() -= other.x();
  y() -= other.y();
}

point& point::operator*= (const double scale) {
  x() *= scale;
  y() *= scale; 
}

point& point::operator/= (const double scale) {
  x() /= scale;
  y() /= scale; 
}

ostream& operator<< (ostream& os, const point& p) {
  os << "(" << p.x() << ", " << p.y() << ")";
  return os; 
}
