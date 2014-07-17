#ifndef STATIC_INIT_HH
#define STATIC_INIT_HH
#include <string> 

class Object;
class RawMaterialProducer;
class RawMaterialHolder;
class UnitHolder; 
using namespace std;

class StaticInitialiser {
public:
  static void initialise ();
  static void loadSave (string fname);
private:
  static void createFactory (Object* fact);
  static void createRawMaterialProducer (Object* def, RawMaterialProducer* rmp);
  static void loadRawMaterials (Object* def, RawMaterialHolder* rmh);
  static void loadUnits (Object* def, UnitHolder* uh); 
};

#endif
