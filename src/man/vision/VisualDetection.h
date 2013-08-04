/**
 * @file   VisualDetection.h
 * @author Tucker Hermans <Tucker@liechtenstein>
 * @date   Mon Feb 16 22:49:04 2009
 *
 * @brief The Abstract super class of many visual objects that can be detected
 * by the vision system
 *
 */

#ifndef VisualDetection_h_defined
#define VisualDetection_h_defined

#include "Structs.h"
#include "NBMath.h"

namespace man {
namespace vision {

class VisualDetection {
public:
    // Constructor
    VisualDetection(int _x = 0, int _y = 0 , float _distance = 0.0,
                    float _bearing = 0.0);
    // Copy constructor
    VisualDetection(const VisualDetection &);
    // Destructor
    virtual ~VisualDetection();

    /* SETTERS */
    // Best Guesses
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    void setCenterX(int cx) { centerX = cx; }
    void setCenterY(int cy) { centerY = cy; }
    void setAngleX(float aX) { angleX = aX; }
    void setAngleY(float aY) { angleY = aY; }
    void setDistance(float d) { distance = d; }
    void setBearing(float b) { bearing = b; }
    void setElevation(float e) { elevation = e; }
    void setX(int x1) {x = x1;}
    void setY(int y1) {y = y1;}
    void setDistanceSD(float _distSD) { distanceSD = _distSD;}
    void setBearingSD(float _bearingSD) { bearingSD = _bearingSD;}
    virtual void setDistanceWithSD(float _distance) = 0;
    //TODO: I'm commenting this out because I modified the method arguments
    //Mexico 2012 - Octavian
//    virtual void setBearingWithSD(float _bearing) = 0;

    void setOn(bool _on){ on = _on; }
    void setTopCam(bool _inTopCam){ inTopCam = _inTopCam; }
    void setFramesOn(int numOn){ framesOn = numOn; }
    void setFramesOff(int numOff){ framesOff = numOff; }

    /* GETTERS */
    const int getX() const {return x;}
    const int getY() const {return y;}
    const point<int> getLocation() const { return point<int>(x, y); }
    const float getWidth() const { return width; }
    const float getHeight() const { return height; }
    const int getCenterX() const { return centerX; }
    const int getCenterY() const { return centerY; }
    const float getAngleX() const { return angleX; }
    const float getAngleY() const { return angleY; }
    const float getAngleXDeg() const { return angleX*TO_DEG; }
    const float getAngleYDeg() const { return angleY*TO_DEG; }
    const float getDistance() const { return distance; }
    const float getBearing() const { return bearing; }
    const float getBearingDeg() const { return bearing*TO_DEG; }
    const float getElevation() const { return elevation; }
    const float getElevationDeg() const { return elevation*TO_DEG; }
    const float getDistanceSD() const { return distanceSD; }
    const float getBearingSD() const { return bearingSD; }

    const bool isOn() const{ return on; }
    const bool isTopCam() const{ return inTopCam;}
    int getFramesOn() { return framesOn; }
    int getFramesOff(){ return framesOff; }

protected:
    /* Best guessed Ball Variables */
    int x, y;
    float width;
    float height;
    int centerX;
    int centerY;
    float angleX;
    float angleY;
    float distance;
    float bearing;
    float elevation;
    // Standard deviation of measurements
    float distanceSD;
    float bearingSD;
    bool on;
    bool inTopCam;
    int framesOn, framesOff;

};

}
}

#endif // VisualDetection_h_defined
