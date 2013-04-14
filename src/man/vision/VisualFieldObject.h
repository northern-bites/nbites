#ifndef VisualFieldObject_hpp_defined
#define VisualFieldObject_hpp_defined

#include <iomanip>
#include <cstdlib>

namespace man {
namespace vision {
	class VisualFieldObject;
}
}

#include "VisualObject.h"
#include "VisualDetection.h"
#include "ConcreteFieldObject.h"
#include "Utility.h"
#include "Structs.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include "Blob.h"
#include "stdio.h"

namespace man {
namespace vision {

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
	void setRedGoalieCertain() {almostCertainlyRedGoalie = true;}
	void setNavyGoalieCertain() {almostCertainlyNavyGoalie = true;}
	void setRedGoalieProbable() {probablyRedGoalie = true;}
	void setNavyGoalieProbable() {probablyNavyGoalie = true;}
    void setDistanceWithSD(float _distance);
    void setBearingWithSD(float _bearing, float _distance);
    virtual void setIDCertainty(certainty c);


    // GETTERS
    const int getLeftTopX() const{ return leftTop.x; }
    const int getLeftTopY() const{ return leftTop.y; }
    const int getRightTopX() const{ return rightTop.x; }
    const int getRightTopY() const{ return rightTop.y; }
    const int getLeftBottomX() const{ return leftBottom.x; }
    const int getLeftBottomY() const{ return leftBottom.y; }
    const int getRightBottomX() const{ return rightBottom.x; }
    const int getRightBottomY() const{ return rightBottom.y; }
	const bool getRedGoalieCertain() const{ return almostCertainlyRedGoalie;}
	const bool getNavyGoalieCertain() const{ return almostCertainlyNavyGoalie;}
	const bool getRedGoalieProbable() const{ return probablyRedGoalie;}
	const bool getNavyGoalieProbable() const{ return probablyNavyGoalie;}
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

    bool isEstimateCertain() const { return estimateCertain; }
    void setEstimateCertain(bool certainty) { estimateCertain = certainty; }

	virtual const bool hasPositiveID();
	virtual bool hasValidDistance() const {
	    //TODO: we should
	    //really have a unified way of telling
	    //if a post estimate is valid or not
	    return getDistance() > 0.0f && estimateCertain;
	}

private: // Class Variables

    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;
    point <float> fieldLocation;
	point <float> fieldLocation2; // for abstract field objects
	bool probablyNavyGoalie;
	bool probablyRedGoalie;
	bool almostCertainlyNavyGoalie;
	bool almostCertainlyRedGoalie;

	bool estimateCertain;

    // This list will hold all the possibilities for this objects's specific ID
    const std::list <const ConcreteFieldObject *> * possibleFieldObjects;

    // Helper Methods
    // Obtained by magic by Octavian and Lizzie summer 2012
    inline static float postDistanceToSD(float _distance) {
        return (_distance-100.f)*(_distance-100.f)/4500.f + 10.f;
    }
    inline static float postBearingToSD(float _distance) {
        return .0000002f*(_distance*_distance) + 4.f * TO_RAD;
    }
    const static float BOTH_UNSURE_DISTANCE_SD;
};

}
}

#endif
