#ifndef VisualBall_h_DEFINED
#define VisualBall_h_DEFINED

#include "Common.h"
#include "Structs.h"
#include "VisionDef.h"
#include "VisualDetection.h"

class VisualBall; // forward reference

// BALL CONSTANTS
#define BallAt1M         16 // pixel width of objects one meter away.
#define PinkBallAt1M     12.46268657f // pixel width of PINK one meter away.
#define MAXBALLDISTANCE  300

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

    // calibration pre-huge chown changes
    //void setFocalDistance() {focDist = 2250*pow((getRadius()*2),-1.0917);}

//obsolete - we don't use this anymore
// TODO: change ball size for sim
#if ROBOT(NAO_SIM)
    void setFocalDistanceFromRadius() { focDist = 100 * 24.5/(getRadius() *2); }
#elif ROBOT(NAO_RL)
    void setFocalDistanceFromRadius()
        {
            focDist = ( 6.4f/8.6f) * (100.f / 83.f) * (50.f / 52.f ) *
                2850.f / (getRadius() * 2.f);
        }
#endif
    void findPinkBlobDist() {
        focDist = PinkBallAt1M * 100 / (getRadius() * 2);
    }

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
