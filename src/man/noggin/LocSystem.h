/**
 * @brief Defines an interface for a localization system. Current implementing classes
 * are LocEKF and MCL
 *
 * @author Tucker Hermans
 */

#ifndef LocSystem_h_DEFINED
#define LocSystem_h_DEFINED
#include <vector>
#include "NogginStructs.h"
#include "Observation.h"

class LocSystem
{
public:
    LocSystem() : active(false), probability(0.0) {};
    virtual ~LocSystem() {};
    // Core Functions
    virtual void updateLocalization(const MotionModel& u_t,
                                    const std::vector<PointObservation>& pt_z,
                                    const std::vector<CornerObservation>& c_z) = 0;
    virtual void reset() = 0;

    virtual void blueGoalieReset() = 0;
    virtual void redGoalieReset() = 0;
    virtual void resetLocTo(float x, float y, float h) = 0;
    virtual void resetLocTo(float x, float y, float h, float x_, float y_, float h_) = 0;


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

protected:
    double probability;
};

#endif // LocSystem_h_DEFINED
