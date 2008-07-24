#ifndef VisualLandmark_h_defined__
#define VisualLandmark_h_defined__

/*
 * The abstract superclass to any object vision can identify.  Subclasses 
 * include VisualCorner and VisualFieldObject.
 */
class VisualLandmark;

#include "ConcreteLandmark.h"
#include "Structs.h"

enum certainty {
  NOT_SURE,
  MILDLY_SURE,
  _SURE
};


class VisualLandmark {


public:
  // Constructor
  VisualLandmark(int _x = 0, int _y = 0, double _distance = 0.0, 
                 double _bearing = 0.0,
                 certainty _idCertainty = NOT_SURE,
                 certainty _distanceCertainty = NOT_SURE,
                 ConcreteLandmark * _concreteLandmark = 0);
  
  // Copy constructor
  VisualLandmark(const VisualLandmark&);

  // Destructor
  virtual ~VisualLandmark();


private:
  // image coordinate system, not field
  int x, y;
  double distance;
  double bearing;
  // How sure are we that we identified the correct object 
  // (for example, blue goal left post versus blue goal right post)
  certainty idCertainty;
  certainty distanceCertainty;
  ConcreteLandmark * concreteLandmark;

public:
  // Getters
  const int getX() const { return x; }
  const int getY() const { return y; }
  const point<int> getLocation() const { return point<int>(x, y); }
  const double getDistance() const { return distance; }
  const double getBearing() const { return bearing; }
  const certainty getIDCertainty() const { return idCertainty; }
  const certainty getDistanceCertainty() const { return distanceCertainty; }
  const ConcreteLandmark * getConcreteLandmark() const { 
    return concreteLandmark; 
  }


  // Setters
  void setX(int _x) { x = _x; }
  void setY(int _y) { y = _y; }
  void setDistance(double _distance) { distance = _distance; }
  void setBearing(double _bearing) { bearing = _bearing; }
  void setIDCertainty(certainty c) { idCertainty = c; }
  void setDistanceCertainty(certainty c) { distanceCertainty = c; }
  void setConcreteLandmark(ConcreteLandmark * c) { concreteLandmark = c; }
};


#endif
