#ifndef Ball_h_DEFINED
#define Ball_h_DEFINED

#include "Common.h"
#include "ifdefs.h"
#include "Structs.h"

class Ball; // forward reference
#include "Vision.h"

// BALL CONSTANTS
#define BallAt1M         16 // pixel width of objects one meter away.
#define PinkBallAt1M     12.46268657 // pixel width of PINK one meter away.
#define MAXBALLDISTANCE  300

class Ball {
public:
    Ball(Vision *vis);
    virtual ~Ball() {}

    // Ball VOID Functions
    void init();

    /* SETTERS */
    // Best Guesses
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    void setRadius(float r) { radius = r; }
    void setCenterX(int cx) { centerX = cx; }
    void setCenterY(int cy) { centerY = cy; }
    void setAngleX(float aX) { angleX = aX; }
    void setAngleY(float aY) { angleY = aY; }
    void setBearing(float b) { bearing = b; }
    void setElevation(float e) { elevation = e; }
    void setConfidence(int c) {confidence = c;}
    void setDistance();
    void setX(int x1) {x = x1;}
    void setY(int y1) {y = y1;}

    void findAngles() {
        setAngleX((IMAGE_WIDTH/2 - centerX) / MAX_BEARING);
        setAngleY((IMAGE_HEIGHT/2 - centerY) / MAX_ELEVATION);
    }
    // calibration pre-huge chown changes
    //void setFocalDistance() {focDist = 2250*pow((getRadius()*2),-1.0917);}
#if ROBOT(NAO_SIM)
    void setFocalDistance() { focDist = 100 * 24.5/(getRadius() *2); }
#elif ROBOT(NAO_RL)
    void setFocalDistance() { focDist = 5700 / (getRadius() * 2); }
#else
    void setFocalDistance() { focDist = 2067.6*pow(getRadius()*2,-1.0595); }
#endif
    void findPinkBlobDist(){focDist = PinkBallAt1M * 100 / (getRadius() * 2); }

    /* GETTERS */
    int getX() {return x;}
    int getY() {return y;}
    float getWidth() { return width; }
    float getHeight() { return height; }
    float getRadius() { return radius; }
    int getCenterX() { return centerX; }
    int getCenterY() { return centerY; }
    float getAngleX() { return angleX; }
    float getAngleY() { return angleY; }
    float getFocDist() { return focDist; }
    float getDist() { return dist; }
    float getBearing() { return bearing; }
    float getElevation() { return elevation; }
    int getConfidence() {return confidence;}

private:
    // Vision class pointer
    Vision *vision;

    /* Best guessed Ball Variables */
    int x, y;
    float width;
    float height;
    float radius;
    int centerX;
    int centerY;
    float angleX;
    float angleY;
    float focDist;
    float dist;
    float bearing;
    float elevation;
    int confidence;
};


#endif // Ball_h_DEFINED
