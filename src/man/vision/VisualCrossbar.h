#ifndef VisualCrossbar_hpp_defined
#define VisualCrossbar_hpp_defined

#include <iomanip>
#include <cstdlib>

namespace man {
namespace vision {
	class VisualCrossbar;
}
}

#include "VisualLandmark.h"
#include "VisualDetection.h"
#include "ConcreteFieldObject.h"
#include "Utility.h"
#include "Structs.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"

namespace man {
namespace vision {

class VisualCrossbar : public VisualDetection {

public:
    // Construcotrs
    VisualCrossbar();
    // copy constructor
    VisualCrossbar(const VisualCrossbar&);

    // Destructor
    virtual ~VisualCrossbar() {}

    friend std::ostream& operator<< (std::ostream &o,
                                     const VisualCrossbar &l)
        {
            return o << "\tWidth: " << l.getWidth()
                     << "\tHeight: " << l.getHeight()
                     << "\tDistance: " << l.getDistance()
                     << "\tAngle X: " << l.getAngleX() << "\tAngle Y: "
                     << l.getAngleY();
        }

    // INITIALIZATION (happens every frame)
    void init();
    void updateCrossbar(blob *b);
    // SETTERS
    void setLeftTopX(int _x){  leftTop.x = _x; }
    void setLeftTopY(int _y){  leftTop.y = _y; }
    void setRightTopX(int _x){ rightTop.x = _x; }
    void setRightTopY(int _y){ rightTop.y = _y; }
    void setLeftBottomX(int _x){ leftBottom.x = _x; }
    void setLeftBottomY(int _y){ leftBottom.y = _y; }
    void setRightBottomX(int _x){ rightBottom.x = _x; }
    void setRightBottomY(int _y){ rightBottom.y = _y; }
    void setShoot(bool s1) {shoot = s1;}
    void setBackLeft(int x1) {backLeft = x1;}
    void setBackRight(int y1) {backRight = y1;}
    void setBackDir(int x1) {backDir = x1;}
    void setLeftOpening(int op) { leftOpening = op; }
    void setRightOpening(int op) { rightOpening = op; }
    void setDistanceWithSD(float _distance);
    void setBearingWithSD(float _bearing);

    // GETTERS
    const int getLeftTopX() const{ return leftTop.x; }
    const int getLeftTopY() const{ return leftTop.y; }
    const int getRightTopX() const{ return rightTop.x; }
    const int getRightTopY() const{ return rightTop.y; }
    const int getLeftBottomX() const{ return leftBottom.x; }
    const int getLeftBottomY() const{ return leftBottom.y; }
    const int getRightBottomX() const{ return rightBottom.x; }
    const int getRightBottomY() const{ return rightBottom.y; }
    const int getShootLeft() const { return backLeft; }
    const int getShootRight() const { return backRight; }
    const int getBackDir() const { return backDir; }
    const int getLeftOpening() const { return leftOpening; }
    const int getRightOpening() const { return rightOpening; }
    const bool shotAvailable() const { return shoot; }

private: // Class Variables

    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;

    // Should be moved to different class
    int backLeft;
    int backRight;
    int backDir;
    int leftOpening;
    int rightOpening;
    bool shoot;

    float crossbarDistanceToSD(float _distance) { return 1.0f; }
    float crossbarBearingToSD(float _bearing) { return 0.01f; }
};

}
}

#endif // VisualCrossbar_hpp_defined
