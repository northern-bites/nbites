/**
 * @brief  Defines a particle for localization purposes.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include "LocStructs.h"

#include <iostream>
#include "Common.pb.h"

namespace man
{
    namespace localization
    {
    /**
     * @class Particle
     * @brief Defines a localization particle with a weight
     *        (not normalized), and a complete location, which
     *        represents a single localization hypothesis.
     */
    class Particle
    {
    public:
        Particle(messages::RobotLocation location_, float weight_)
            : weight(weight_), location(location_) { }

        Particle() : weight(0.0f), location() {}

        ~Particle() { }

        messages::RobotLocation getLocation() const { return location; }
        void setLocation(messages::RobotLocation location_) { location = location_; }

        float getWeight() const { return weight; }
        void setWeight(float weight_) { weight = weight_; }

        void setX(float x) { location.set_x(x); }
        void setY(float y) { location.set_y(y); }
        void setH(float h) { location.set_h(h); }

        void shiftParticle(messages::RobotLocation shiftAmount)
        {
            location.set_x(location.x() + shiftAmount.x());
            location.set_y(location.y() + shiftAmount.y());
            location.set_h(location.h() + shiftAmount.h());
        }

        void normalizeWeight(float totalWeight) {weight = weight/totalWeight;}

        /**
         * @brief Used to compare two particles by weight, primarily for
         *        use with the sorting algorithm.
         *
         * @param first the first particle.
         * @param second the second particle.
         * @return weight of first < weight of second ? true : false
         */
        friend bool operator <(const Particle& first,
                               const Particle& second)
        {
            float w1 = first.getWeight();
            float w2 = second.getWeight();

            return (w1 < w2) ? true : false;
        }

        friend std::ostream& operator<<(std::ostream& out,
                                        Particle p)
        {
            out << "Particle with weight " << p.getWeight() << " with "
                << p.getLocation().DebugString() << std::endl;
            return out;
        }

    private:
        float weight;                           //! The particle weight.
        messages::RobotLocation location;  //! The particle location (x, y, heading).

    };

    /*
     * @brief The total weights of a Particle Set should sum to 1 for resampling
     */
    typedef std::vector<Particle> ParticleSet;
    typedef ParticleSet::iterator ParticleIt;
    } // namespace localization
} // namespace man
