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
 * Class for applying exponential filtering to the accelerometer values
 * we receive from sensors (see: ZmpEKF.h)
 **/

#ifndef _ZmpExp_h_DEFINED
#define _ZmpExp_h_DEFINED

#include "EKFStructs.h" // for ZmpMeasurement

#include "ExponentialFilter.h"

#define ZMP_EXP_X 0
#define ZMP_EXP_Y 1

const unsigned int zmp_num_dimensions = 2;

class ZmpExp : public ExponentialFilter<ZmpMeasurement, zmp_num_dimensions>

{
public:
	ZmpExp();
	virtual ~ZmpExp();

	void update(const ZmpTimeUpdate tUp,
				const ZmpMeasurement zMeasure);

	// getters
	const float get_zmp_x() { return getFilteredElement(ZMP_EXP_X); }
	const float get_zmp_y() { return getFilteredElement(ZMP_EXP_Y); }

private:
	virtual void incorporateMeasurement(ZmpMeasurement z);
};

#endif
