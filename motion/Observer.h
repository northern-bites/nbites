
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
 * This class implements the 1D controller described by Kajita and Czarnetzki
 * Each discrete time step, the tick method is called with the latest
 * previewable ZMP_REF positions.
 * Important: This controller models only one dimension at once, so you need
 * two instances one for the x and one for the y direction.
 * The weights and the time invariant system matrix A (see constructor, etc)
 * are pre-calculated in Octave (see observer.m and setupobserver.m). The
 * theory is described in Czarnetzki and Kajita and Katayama.
 *
 * @author George Slavov
 * @author Johannes Strom
 * @date March 2009
 */


#ifndef _Observer_h_DEFINED
#define _Observer_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <list>

#include "NBMatrixMath.h"
#include "WalkController.h"
#include "motionconfig.h"

#ifdef DEBUG_MOTION
#  define DEBUG_CONTROLLER_GAINS
#endif

class Observer : public WalkController {
public:
    Observer();
    virtual ~Observer(){};
    virtual const float tick(const std::list<float> *zmp_ref,
                             const float cur_zmp_ref,
                             const float sensor_zmp);
    virtual const float getPosition() const { return stateVector(0); }
    virtual const float getZMP() const {return stateVector(2);}

    virtual void initState(float x, float v, float p);
private:
    NBMath::ufvector3 stateVector;

public: //Constants
    static const unsigned int NUM_PREVIEW_FRAMES = 70;
    static const unsigned int NUM_AVAIL_PREVIEW_FRAMES = 120;
private:
    static const float weights[NUM_AVAIL_PREVIEW_FRAMES];
    static const float A_values[9];
    static const float b_values[3];
    static const float c_values[3];
    static const float L_values[3];
    static const float Gi;

    NBMath::ufmatrix3 A;
    NBMath::ufvector3 b;
    NBMath::ufrowVector3 c;
    NBMath::ufvector3 L;

    float trackingError;
};

#endif
