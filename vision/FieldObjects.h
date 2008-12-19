#ifndef FieldObjects_h_DEFINED
#define FieldObjects_h_DEFINED

#include <string>
using namespace std;

#include "Common.h"
#include "Structs.h"
#include "ifdefs.h"
#include "FieldConstants.h"
#include "ConcreteFieldObject.h"

class FieldObjects;
#include "Vision.h"

class FieldObjects {
public:
    //FieldObjects(Vision *vis, const fieldObjectID);
    //FieldObjects(Vision *vis);
    FieldObjects(const fieldObjectID);
    FieldObjects();
    virtual ~FieldObjects() {}

    friend std::ostream& operator<< (std::ostream &o, const FieldObjects &l)
        {
            return o << l.toString() << "\tWidth: " << l.width
                     << "\tHeight: " << l.height
                     << "\tDistance: " << l.dist
                     << "\tAngle X: " << l.angleX << "\tAngle Y: " << l.angleY;
        }

    // INITIALIZATION (happens every frame)
    void init();

    void printDebugInfo(FILE * out);
    static string getStringFromID(fieldObjectID _id);

    // SETTERS
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    void setX(int x1) { x = x1; }
    void setY(int y1) { y = y1; }
    void setLeftTopX(int _x){  leftTop.x = _x; }
    void setLeftTopY(int _y){  leftTop.y = _y; }
    void setRightTopX(int _x){ rightTop.x = _x; }
    void setRightTopY(int _y){ rightTop.y = _y; }
    void setLeftBottomX(int _x){ leftBottom.x = _x; }
    void setLeftBottomY(int _y){ leftBottom.y = _y; }
    void setRightBottomX(int _x){ rightBottom.x = _x; }
    void setRightBottomY(int _y){ rightBottom.y = _y; }
    void setCenterX(int cX) { centerX = cX; }
    void setCenterY(int cY) { centerY = cY; }
    void setAngleX(float aX) { angleX = aX; }
    void setAngleY(float aY) { angleY = aY; }
    void setBearing(float b) { bearing = b; }
    void setElevation(float e) { elevation = e; }
    void setFocDist(float fd) { focDist = fd; }
    void setDist(float d) { dist = d; }
    void setCertainty(int c) {certainty = c; }
    void setDistCertainty(int c) {distCertainty = c;}
    void setShoot(bool s1) {shoot = s1;}
    void setBackLeft(int x1) {backLeft = x1;}
    void setBackRight(int y1) {backRight = y1;}
    void setBackDir(int x1) {backDir = x1;}
    void setLeftOpening(int op) { leftOpening = op; }
    void setRightOpening(int op) { rightOpening = op; }

    // GETTERS
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    int getX() const{ return x; }
    int getY() const{ return y; }
    int getLeftTopX() const{ return leftTop.x; }
    int getLeftTopY() const{ return leftTop.y; }
    int getRightTopX() const{ return rightTop.x; }
    int getRightTopY() const{ return rightTop.y; }
    int getLeftBottomX() const{ return leftBottom.x; }
    int getLeftBottomY() const{ return leftBottom.y; }
    int getRightBottomX() const{ return rightBottom.x; }
    int getRightBottomY() const{ return rightBottom.y; }
    int getCenterX() const { return centerX; }
    int getCenterY() const { return centerY; }
    int getCertainty() const { return certainty; }
    int getDistCertainty() const { return distCertainty; }
    float getAngleX() const { return angleX; }
    float getAngleY() const { return angleY; }
    float getFocDist() const { return focDist; }
    float getDist() const{ return dist; }
    float getBearing() const { return bearing; }
    float getElevation() const { return elevation; }
    int getShootLeft() const { return backLeft; }
    int getShootRight() const { return backRight; }
    int getBackDir() const { return backDir; }
    int getLeftOpening() const { return leftOpening; }
    int getRightOpening() const { return rightOpening; }
    bool shotAvailable() const { return shoot; }
    string toString() const { return getStringFromID(id); }
    const point<float> getFieldLocation() const { return fieldLocation; }
    const float getFieldX() const { return fieldLocation.x; }
    const float getFieldY() const { return fieldLocation.y; }
    const fieldObjectID getID() const { return id; }

    static const float getHeightFromGround(const fieldObjectID id);
    // static const bool isBeacon(const FieldObjects * obj);
    // static const bool isGoal(const FieldObjects * obj);
    // static const bool isArc(const FieldObjects * obj);




public:
    static const FieldObjects blue_goal_left_post, blue_goal_right_post,
                              yellow_goal_left_post, _goal_right_post,
                              yellow_blue_beacon, blue_yellow_beacon;

    static const float WHITE_HEIGHT_ON_BEACON;

private:
    Vision *vision;

    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;

    float width;
    float height;
    int x;
    int y;
    int centerX;
    int centerY;
    int certainty;
    int distCertainty;
    int backLeft;
    int backRight;
    int backDir;
    int leftOpening;
    int rightOpening;
    bool shoot;
    float angleX;
    float angleY;
    float focDist;
    float dist;
    float bearing;
    float elevation;
    point <float> fieldLocation;
    fieldObjectID id;
};

#endif // FieldObjects_h_DEFINED
