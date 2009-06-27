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
#ifndef _AngleEKF_h_DEFINED
#define _AngleEKF_h_DEFINED

/**
 * Class for filtering angle values received from sensors. Extends the
 * abstract EKF class.
 */

#include "EKF.h"
#include "EKFStructs.h"

class AngleEKF : public EKF<AngleMeasurement, int, ANGLE_NUM_DIMENSIONS,
                          ANGLE_NUM_DIMENSIONS>
{
public:
    AngleEKF();
    virtual ~AngleEKF();

    // Update functions
    void update(const float anglex,
                const float angley);

    // getters
    const float getAngleX() const { return xhat_k(0); }
    const float getAngleY() const { return xhat_k(1); }
    const float getAngleXUnc() const { return P_k(0,0); }
    const float getAngleYUnc() const { return P_k(1,1); }


private:
    // Core functions
    virtual StateVector associateTimeUpdate(int u_k);
    virtual void incorporateMeasurement(AngleMeasurement z,
                                        StateMeasurementMatrix &H_k,
                                        MeasurementMatrix &R_k,
                                        MeasurementVector &V_k);
    const float scale(const float);
    const float getVariance(float, float);

private: // Constants
    static const int num_dimensions;
    static const float beta;
    static const float gamma;
    static const float variance;
};
#endif
