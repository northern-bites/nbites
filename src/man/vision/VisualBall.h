#ifndef VisualBall_h_DEFINED
#define VisualBall_h_DEFINED

#include "Common.h"
#include "Structs.h"
#include "VisionDef.h"
#include "VisualDetection.h"

class VisualBall : public VisualDetection
{
public:
    VisualBall();
    virtual ~VisualBall() {}

    // VisualBall VOID Functions
    void init();

    // Setters
    void setRadius(float r) { radius = r; }
    void setConfidence(int c) {confidence = c;}
    void setDistanceEst(estimate ball_est);
    void setDistanceWithSD(float _dist);
    void setBearingWithSD(float b);
    void findAngles() {
      setAngleX( static_cast<float>((IMAGE_WIDTH/2 - centerX)) /
		 static_cast<float>(IMAGE_WIDTH/2) *
		 MAX_BEARING_RAD);
      
      setAngleY( static_cast<float>(IMAGE_HEIGHT/2 - centerY) /
		 static_cast<float>(IMAGE_HEIGHT/2) *
		 MAX_ELEVATION_RAD);
    }
	void setHeat(float value) {heat = value;}

    // Getters
    const float getRadius() const { return radius; }
    const int getConfidence() const { return confidence;}
    const float getHeat() const { return heat;}
    
    // Member functions
    const float ballDistanceToSD(float _distance) const {
      return static_cast<float>(sqrt(10.f + _distance * 0.2f));
    }
    const float ballBearingToSD(float _bearing) const {
      return static_cast<float>(sqrt(static_cast<float>(M_PI) / 4.0f));
    }
    
 private:
    float radius;
    int confidence;
    float heat;
    
};


#endif // VisualBall_h_DEFINED
