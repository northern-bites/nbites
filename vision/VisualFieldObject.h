#ifndef VisualFieldObject_hpp_defined
#define VisualFieldObject_hpp_defined

#include <iomanip>
#include <cstdlib>

class VisualFieldObject;

#include "VisualLandmark.h"
#include "ConcreteFieldObject.h"
#include "Utility.h"
#include "Structs.h"


class VisualFieldObject : public VisualLandmark {

 public:
  VisualFieldObject(const int _x, const int _y, const float _distance,
                    const float _bearing);
  // destructor
  virtual ~VisualFieldObject();
  // copy constructor
  VisualFieldObject(const VisualFieldObject&);

  friend std::ostream& operator<< (std::ostream &o, const VisualFieldObject &c)
  {
    return o << setprecision(2)
             << "(" << c.getX() << "," << c.getY() << ") \tDistance: "
             << c.getDistance() << "\tBearing: " << c.getBearing();
  }

  ////////////////////////////////////////////////////////////
  // GETTERS
  ////////////////////////////////////////////////////////////
  const list <const ConcreteFieldObject *> getPossibleFieldObjects() const {
    return possibleFieldObjects; }

  ////////////////////////////////////////////////////////////
  // SETTERS
  ////////////////////////////////////////////////////////////
  void setPossibleFieldObjects(list <const ConcreteFieldObject *>
                               _possibleFieldObjects) {
      possibleFieldObjects = _possibleFieldObjects; }

private:
  // This list will hold all the possibilities for this corner's specific ID
  // It will get set from within FieldLines.cc.
  list <const ConcreteFieldObject *> possibleFieldObjects;
};

#endif
