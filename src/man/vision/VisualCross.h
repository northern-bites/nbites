#ifndef VisualCross_hpp_defined
#define VisualCross_hpp_defined

#include <iomanip>
#include <cstdlib>

#include "VisualObject.h"
#include "ConcreteFieldObject.h"
#include "Utility.h"
#include "Structs.h"
#include "VisionStructs.h"
#include "Blob.h"
#include "VisionHelpers.h"
#include "ConcreteCross.h"

// Values for the Standard Deviation calculations

class VisualCross : public VisualObject {

public:
    // ConcreteType provided by this VisualObject
    typedef ConcreteCross ConcreteType;

    // Construcotrs
    VisualCross();
	VisualCross(const crossID id);
    // copy constructor
    VisualCross(const VisualCross&);

    // Destructor
    virtual ~VisualCross() {}

    friend std::ostream& operator<< (std::ostream &o,
                                     const VisualCross &l)
        {
            return o << "\tWidth: " << l.getWidth()
                     << "\tHeight: " << l.getHeight()
                     << "\tDistance: " << l.getDistance()
                     << "\tAngle X: " << l.getAngleX() << "\tAngle Y: "
                     << l.getAngleY();
        }

    // INITIALIZATION (happens every frame)
    void init();

    // SETTERS
    void setLeftTopX(int _x){  leftTop.x = _x; }
    void setLeftTopY(int _y){  leftTop.y = _y; }
    void setRightTopX(int _x){ rightTop.x = _x; }
    void setRightTopY(int _y){ rightTop.y = _y; }
    void setLeftBottomX(int _x){ leftBottom.x = _x; }
    void setLeftBottomY(int _y){ leftBottom.y = _y; }
    void setRightBottomX(int _x){ rightBottom.x = _x; }
    void setRightBottomY(int _y){ rightBottom.y = _y; }
    void setDistanceWithSD(float _distance);
    void setBearingWithSD(float _bearing);
    void updateCross(Blob *b);
    void setPossibleCrosses(const std::list <const ConcreteCross *> *
                            _possibleCrosses) {
        possibleCrosses = _possibleCrosses;
    }
	void setID(crossID _id);

    // GETTERS
    const int getLeftTopX() const{ return leftTop.x; }
    const int getLeftTopY() const{ return leftTop.y; }
    const int getRightTopX() const{ return rightTop.x; }
    const int getRightTopY() const{ return rightTop.y; }
    const int getLeftBottomX() const{ return leftBottom.x; }
    const int getLeftBottomY() const{ return leftBottom.y; }
    const int getRightBottomX() const{ return rightBottom.x; }
    const int getRightBottomY() const{ return rightBottom.y; }
    const std::list <const ConcreteCross *> * getPossibilities() const {
        return possibleCrosses;
    }
	virtual const bool hasPositiveID();

private: // Class Variables

    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;
    // This list will hold all the possibilities for this objects's specific ID
    const std::list <const ConcreteCross *> * possibleCrosses;

    // Member functions
    inline static float robotDistanceToSD(float _distance) {
        return sqrtf(0.00000004f * std::pow(_distance,4.079f));
    }
    inline static float robotBearingToSD(float _bearing) {
        return sqrtf(M_PI_FLOAT / 8.0f);
    }

};

#endif // VisualCross_hpp_defined
