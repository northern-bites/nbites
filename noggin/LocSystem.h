/**
 * LocSystem.h - Header file for the simple abstract Loc System class
 *
 * @authro Tucker Hermans
 */

#ifndef LocSystem_h_DEFINED
#define LocSystem_h_DEFINED
#include <vector>
#include "NogginStructs.h"
#include "Observation.h"

class LocSystem
{
public:
    virtual ~LocSystem() {};
    // Core Functions
    virtual void updateLocalization(MotionModel u_t,
                                    std::vector<Observation> z_t) = 0;
    virtual void reset() = 0;

    // Getters
    virtual const PoseEst getCurrentEstimate() const = 0;
    virtual const PoseEst getCurrentUncertainty() const = 0;
    virtual const float getXEst() const = 0;
    virtual const float getYEst() const = 0;
    virtual const float getHEst() const = 0;
    virtual const float getHEstDeg() const = 0;
    virtual const float getXUncert() const = 0;
    virtual const float getYUncert() const = 0;
    virtual const float getHUncert() const = 0;
    virtual const float getHUncertDeg() const = 0;
    virtual const MotionModel getLastOdo() const = 0;

    // Setters
    virtual void setXEst(float xEst) = 0;
    virtual void setYEst(float yEst) = 0;
    virtual void setHEst(float hEst) = 0;
    virtual void setXUncert(float uncertX) = 0;
    virtual void setYUncert(float uncertY) = 0;
    virtual void setHUncert(float uncertH) = 0;

    friend std::ostream& operator<< (std::ostream &o,
                                             const LocSystem &c) {
        return o << "Est: (" << c.getXEst() << ", " << c.getYEst() << ", "
                 << c.getHEst() << ")\t"
                 << "Uncert: (" << c.getXUncert() << ", " << c.getYUncert()
                 << ", "
                 << c.getHUncert() << ")";

    }
};

#endif // LocSystem_h_DEFINED
