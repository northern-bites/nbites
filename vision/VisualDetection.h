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

class VisualDetection {
public:
    VisualDetection() {}
    virtual ~VisualDetection() {}

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
    void findAngles() {
        setAngleX((IMAGE_WIDTH/2 - centerX) / MAX_BEARING);
        setAngleY((IMAGE_HEIGHT/2 - centerY) / MAX_ELEVATION);
    }

    /* GETTERS */
    const int getX() const {return x;}
    const int getY() const {return y;}
    const float getWidth() const { return width; }
    const float getHeight() const { return height; }
    const int getCenterX() const { return centerX; }
    const int getCenterY() const { return centerY; }
    const float getAngleX() const { return angleX; }
    const float getAngleY() const { return angleY; }
    const float getDistance() const { return distance; }
    const float getBearing() const { return bearing; }
    const float getElevation() const { return elevation; }
    const float getDistanceSD() const { return distanceSD; }
    const float getBearingSD() const { return bearingSD; }

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

};

#endif // VisualDetection_h_defined
