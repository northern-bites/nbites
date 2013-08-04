
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

#ifndef CoordFrame_h_DEFINED
#define CoordFrame_h_DEFINED

#include "NBMatrixMath.h"
#include "NBMath.h"

//Homogeneous 3D coordinates (really this is 2D
namespace CoordFrame3D{

    enum Axis {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    };

    // -------------------- Helper matrix methods --------------------
    const NBMath::ufmatrix3 rotation3D(const Axis axis,
				       const float angle);

    const NBMath::ufmatrix3 translation3D(const float dx,
					  const float dy);

    const NBMath::ufvector3 vector3D(const float x, const float y,
				     const float z = 1.0f);

    const NBMath::ufrowVector3 rowVector3D(const float x,
                                           const float y,
                                           const float z = 1.0f);

    const NBMath::ufmatrix3 identity3D();
};


namespace CoordFrame4D{
    enum Axis {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS,
        W_AXIS
    };

    // -------------------- Helper matrix methods --------------------

    const NBMath::ufmatrix4
    rotation4D(const Axis axis,
               const float angle);

    const NBMath::ufmatrix4 translation4D(const float dx,
                                          const float dy,
                                          const float dz);

    const NBMath::ufvector4 vector4D(const float x, const float y,
                                     const float z,
                                     const float w = 1.0f);


    /**
     * Returns precalculated Trans[x,y,z].Rotz[wz].Roty[wy].Rotx[wx]
     */
    const NBMath::ufmatrix4 get6DTransform(const float x,
                                           const float y,
                                           const float z,
                                           const float wx,
                                           const float wy,
                                           const float wz);

    const NBMath::ufmatrix4
    invertHomogenous(const NBMath::ufmatrix4 source);

};
#endif
