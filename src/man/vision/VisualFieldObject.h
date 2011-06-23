#ifndef VisualFieldObject_hpp_defined
#define VisualFieldObject_hpp_defined

#include <iomanip>
#include <cstdlib>

class VisualFieldObject;

#include "VisualObject.h"
#include "VisualDetection.h"
#include "ConcreteFieldObject.h"
#include "Utility.h"
#include "Structs.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include "Blob.h"
#include "stdio.h"

class VisualFieldObject : public VisualObject {

public:
    // ConcreteType provided by this VisualObject
    typedef ConcreteFieldObject ConcreteType;

    // Construcotrs
    VisualFieldObject(const int _x, const int _y, const float _distance,
                      const float _bearing);
    VisualFieldObject(const fieldObjectID);
    VisualFieldObject();
    // copy constructor
    VisualFieldObject(const VisualFieldObject&);

    // Destructor
    virtual ~VisualFieldObject() {}

    friend std::ostream& operator<< (std::ostream &o,
                                     const VisualFieldObject &l)
        {
            return o << l.toString() << "\tWidth: " << l.getWidth()
                     << "\tHeight: " << l.getHeight()
                     << "\tDistance: " << l.getDistance()
                     << "\tAngle X: " << l.getAngleX() << "\tAngle Y: "
                     << l.getAngleY();
        }

    // INITIALIZATION (happens every frame)
    void init();
    void printDebugInfo(FILE * out);
    void updateObject(Blob* b, certainty _certainty,
                      distanceCertainty _distCertainty);

    // SETTERS
    void setLeftTopX(int _x){  leftTop.x = _x; }
    void setLeftTopY(int _y){  leftTop.y = _y; }
    void setRightTopX(int _x){ rightTop.x = _x; }
    void setRightTopY(int _y){ rightTop.y = _y; }
    void setLeftBottomX(int _x){ leftBottom.x = _x; }
    void setLeftBottomY(int _y){ leftBottom.y = _y; }
    void setRightBottomX(int _x){ rightBottom.x = _x; }
    void setRightBottomY(int _y){ rightBottom.y = _y; }
    void setPossibleFieldObjects(const std::list <const ConcreteFieldObject *> *
                                 _possibleFieldObjects) {
        possibleFieldObjects = _possibleFieldObjects;
    }
    void setDistanceWithSD(float _distance);
    void setBearingWithSD(float _bearing);
    virtual void setIDCertainty(certainty c);

    void setOn(bool _on){ on = _on; }
    void setFramesOn(int numOn){ framesOn = numOn; }
    void setFramesOff(int numOff){ framesOff = numOff; }

    // GETTERS
    const int getLeftTopX() const{ return leftTop.x; }
    const int getLeftTopY() const{ return leftTop.y; }
    const int getRightTopX() const{ return rightTop.x; }
    const int getRightTopY() const{ return rightTop.y; }
    const int getLeftBottomX() const{ return leftBottom.x; }
    const int getLeftBottomY() const{ return leftBottom.y; }
    const int getRightBottomX() const{ return rightBottom.x; }
    const int getRightBottomY() const{ return rightBottom.y; }
    const std::string toString() const { return ConcreteFieldObject::
            getStringFromID(id); }
    const point<float> getFieldLocation() const { return fieldLocation; }
    const float getFieldX() const { return fieldLocation.x; }
    const float getFieldY() const { return fieldLocation.y; }
    const float getFieldX2() const { return fieldLocation2.x; }
    const float getFieldY2() const { return fieldLocation2.y; }
    const std::list<const ConcreteFieldObject*> * getPossibilities() const {
        return possibleFieldObjects;
    }

    virtual const bool hasPositiveID();
    const bool isOn() const{ return on; }
    int getFramesOn() { return framesOn; }
    int getFramesOff(){ return framesOff; }

private: // Class Variables

    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;
    point <float> fieldLocation;
	point <float> fieldLocation2; // for abstract field objects
    // This list will hold all the possibilities for this objects's specific ID
    const std::list <const ConcreteFieldObject *> * possibleFieldObjects;
    bool on;
    int framesOn, framesOff;

    // Helper Methods
    inline static float postDistanceToSD(float _distance) {
        //return 0.0496f * exp(0.0271f * _distance);
        return sqrtf(2.0f*(10 + (_distance * _distance)*0.00125f));
    }
    inline static float postBearingToSD(float _bearing) {
        return sqrtf(static_cast<float>(M_PI) / 8.0f);
    }
    const static float BOTH_UNSURE_DISTANCE_SD;
};

#endif
