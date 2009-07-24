#ifndef VisualRobot_hpp_defined
#define VisualRobot_hpp_defined

#include <iomanip>
#include <cstdlib>

class VisualRobot;

#include "VisualLandmark.h"
#include "VisualDetection.h"
#include "ConcreteFieldObject.h"
#include "Utility.h"
#include "Structs.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include "Blob.h"

// Values for the Standard Deviation calculations

// This class should eventually inheret from VisualLandmark, once it is
// cleaned a bit
class VisualRobot : public VisualDetection {

public:
    // Construcotrs
    VisualRobot();
    // copy constructor
    VisualRobot(const VisualRobot&);

    // Destructor
    virtual ~VisualRobot() {}

    friend std::ostream& operator<< (std::ostream &o,
                                     const VisualRobot &l)
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
    void updateRobot(Blob b);

    // GETTERS
    const int getLeftTopX() const{ return leftTop.x; }
    const int getLeftTopY() const{ return leftTop.y; }
    const int getRightTopX() const{ return rightTop.x; }
    const int getRightTopY() const{ return rightTop.y; }
    const int getLeftBottomX() const{ return leftBottom.x; }
    const int getLeftBottomY() const{ return leftBottom.y; }
    const int getRightBottomX() const{ return rightBottom.x; }
    const int getRightBottomY() const{ return rightBottom.y; }

private: // Class Variables

    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;

    int backLeft;
    int backRight;
    int backDir;

    // Member functions
    float robotDistanceToSD(float _distance) {
        return 0.00000004f * std::pow(_distance,4.079f);
    }
    float robotBearingToSD(float _bearing) {
        return static_cast<float>(M_PI_FLOAT) / 8.0f;
    }

};

#endif // VisualRobot_hpp_defined
