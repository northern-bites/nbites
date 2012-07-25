/**
 * @brief Defines an interface for a localization system. Current implementing classes
 * are LocEKF and MCL
 *
 * @author Tucker Hermans
 */

#ifndef LocSystem_h_DEFINED
#define LocSystem_h_DEFINED
#include <vector>

#include "ClassHelper.h"

#include "NogginStructs.h"
#include "Observation.h"

struct LocNormalParams
{
    LocNormalParams(float sx, float sy, float sh)
    : sigma_x(sx), sigma_y(sy), sigma_h(sh)
    {
    }
    
    LocNormalParams()
    : sigma_x(15.0), sigma_y(15.0), sigma_h(1.0)
    {
    }

    float sigma_x;         
    float sigma_y;
    float sigma_h;
};

class LocSystem
{
    ADD_SHARED_PTR(LocSystem)

public:
 LocSystem() : active(false), onOpposingSide(false), probability(0.0) {};
    virtual ~LocSystem() {};
    // Core Functions
    virtual void updateLocalization(const MotionModel& u_t,
                                    const std::vector<PointObservation>& pt_z,
                                    const std::vector<CornerObservation>& c_z) = 0;
    virtual void reset() = 0;

    virtual void blueGoalieReset() = 0;
    virtual void redGoalieReset() = 0;
    virtual void resetLocTo(float x, float y, float h,
			    LocNormalParams params = LocNormalParams()) = 0;
    virtual void resetLocTo(float x, float y, float h,
			    float x_, float y_, float h_,
			    LocNormalParams params1 = LocNormalParams(),
			    LocNormalParams params2 = LocNormalParams()) = 0;

    virtual void resetLocToSide(bool blueSide) = 0;

    // Getters
    virtual PoseEst getCurrentEstimate() const    = 0;
    virtual PoseEst getCurrentUncertainty() const = 0;
    virtual float getXEst() const                 = 0;
    virtual float getYEst() const                 = 0;
    virtual float getHEst() const                 = 0;
    virtual float getHEstDeg() const              = 0;
    virtual float getXUncert() const              = 0;
    virtual float getYUncert() const              = 0;
    virtual float getHUncert() const              = 0;
    virtual float getHUncertDeg() const           = 0;
    virtual MotionModel getLastOdo() const        = 0;

    virtual std::vector<PointObservation> getLastPointObservations() const = 0;

    virtual
    std::vector<CornerObservation> getLastCornerObservations() const = 0;

    virtual bool isActive() const { return active;}
    double getProbability() const { return probability; }

    bool isOnOpposingSide() const { return onOpposingSide; }

    // Setters
    virtual void setXEst(float xEst) = 0;
    virtual void setYEst(float yEst) = 0;
    virtual void setHEst(float hEst) = 0;
    virtual void setXUncert(float uncertX) = 0;
    virtual void setYUncert(float uncertY) = 0;
    virtual void setHUncert(float uncertH) = 0;
    virtual void activate() { active = true; }
    virtual void deactivate() { active = false; }
    void setProbability(double p) { probability = p; }
    
    void setOnOpposingSide(bool opp) { onOpposingSide = opp; }


    friend std::ostream& operator<< (std::ostream &o,
                                             const LocSystem &c) {
        return o << "Est: (" << c.getXEst() << ", " << c.getYEst() << ", "
                 << c.getHEst() << ")\t"
                 << "Uncert: (" << c.getXUncert() << ", " << c.getYUncert()
                 << ", "
                 << c.getHUncert() << ")\t"
                 << "Prob: " << c.getProbability();

    }

private:
    bool active;

    // Indicates which side of the field the robot is on.
    // True only if the robot is on the opposing side of the 
    // field.
    bool onOpposingSide;

protected:
    double probability;
};

#endif // LocSystem_h_DEFINED
