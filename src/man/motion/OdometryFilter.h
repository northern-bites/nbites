// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

/**
 * Filter class for odometry velocity measurements. We use a Butterworth
 * filter, the odometry seems fairly stable and periodic except for extremely
 * regular disturbances in X. Not sure if these are caused by some sort of
 * bug in the matrices we use (fc_Transform, primarily) or reflect an actual
 * phenomenon.
 *
 * @author Nathan Merritt
 * @date July 2011 (Istanbul)
 */

#pragma once
#ifndef ODOMETRY_FILETER_DEFINED
#define ODOMETRY_FILETER_DEFINED

#include <boost/shared_ptr.hpp>
#include <vector>

#include "dsp.h"

class OdoFilter : public Filter {
public:
    typedef boost::shared_ptr<OdoFilter> ptr; // in case WalkingLeg needs a ptr

    OdoFilter();

    virtual void Reset();
    void update (float velX, float velY, float velTheta);
    std::vector<float> getOdometry();

    // have to override the pure virtual
    virtual double X(double X) { return X; }

private:
    Butterworth velX, velY, velTheta; // separate filter for each dimension
};

#endif
