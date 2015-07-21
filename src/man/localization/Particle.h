/**
 * @brief  Defines a particle for localization purposes
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   January 2013
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
 */

#pragma once

#include "LocStructs.h"
#include "RobotLocation.pb.h"

#include <iostream>

namespace man
{
namespace localization
{

// A particle stores a robot pose that represents a single localization hypothesis 
// combined with a weight that is set in the sensor update step and used 
// in the resample step
class Particle
{
public:
    // Constructor
    // @param location_, the robot pose
    // @param weight_, the weight used in resampling step
    Particle(messages::RobotLocation& location_, float weight_)
        : location(location_), weight(weight_) { }

    // Constructor
    // @param x_, the x component of robot pose
    // @param y_, the y component of robot pose
    // @param h_, the h component of robot pose
    // @param weight_, the weight used in resampling step
    Particle(float x_, float y_, float h_, float weight_) {
        location.set_x(x_);
        location.set_y(y_);
        location.set_h(h_);
        weight = weight_;
    }

    // Default constructor
    Particle() : location(), weight(0.0f) {}

    // Destructor
    ~Particle() {}

    // Getters and setters for robot pose
    const messages::RobotLocation& getLocation() const { return location; }
    void setLocation(messages::RobotLocation& location_) { location = location_; }

    // Getters and setters for particle weight
    float getWeight() const { return weight; }
    void setWeight(float weight_) { weight = weight_; }

    // Shift particle in x, y, and h
    // @param shiftAmount, the amount to shift particle as robot location
    void shift(messages::RobotLocation& shiftAmount) {
        location.set_x(location.x() + shiftAmount.x());
        location.set_y(location.y() + shiftAmount.y());
        location.set_h(NBMath::subPIAngle(location.h() + shiftAmount.h()));
    }

    // Shift particle in x, y, and h
    // @param shiftX, amount to shift in x
    // @param shiftY, amount to shift in y
    // @param shiftH, amount to shift in h
    void shift(float shiftX, float shiftY, float shiftH) {
        location.set_x(location.x() + shiftX);
        location.set_y(location.y() + shiftY);
        location.set_h(NBMath::subPIAngle(location.h() + shiftH));
    }

    // Normalize weight of particle, so that sum of all particles' weights is zero
    // @param totalWeight, sum of all particles' weights
    void normalizeWeight(float totalWeight) { weight = weight / totalWeight; }

     // Used to compare two particles by weight, primarily for use with 
     // std sorting algorithm
     // @param first, the first particle.
     // @param second, the second particle.
     // @returns (weight of first < weight of second ? true : false)
    friend bool operator <(const Particle& first,
                           const Particle& second) {
        float w1 = first.getWeight();
        float w2 = second.getWeight();
        return (w1 < w2) ? true : false;
    }

private:
    messages::RobotLocation location;
    float weight;
};

typedef std::vector<Particle> ParticleSet;
typedef ParticleSet::iterator ParticleIt;

} // namespace localization
} // namespace man
