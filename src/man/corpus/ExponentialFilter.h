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
 * @brief Exponential filter on an arbitrarily sized vector. Each update timestep
 * applies <vector T> = (alpha)<vector T-1> + (1-alpha)<sensor values>
 *
 * @date Septempter 2010
 * @author Nathan Merritt
 **/

#ifndef _ExponentialFilter_h_DEFINED
#define _ExponentialFilter_h_DEFINED

#include <boost/numeric/ublas/vector.hpp>

const float DEFAULT_ALPHA = 0.85f;

template <class Measurement, unsigned int dimension>

class ExponentialFilter
{
public:
	typedef boost::numeric::ublas::vector<float, boost::numeric::ublas::
                                          bounded_array<float,dimension> >
	StateVector;

protected:
	// how much of the current vector is kept in each update timestep
	float alpha;
	StateVector currentState;

public:
	// Constructors & Destructors
	ExponentialFilter()
		: alpha(DEFAULT_ALPHA), currentState(dimension)
	{
		zeroCurrentState();
	}

	ExponentialFilter(float _alpha)
		: alpha(_alpha), currentState(dimension)
	{
		zeroCurrentState();
	}

	virtual ~ExponentialFilter() {}

	// update function
	void updateStep(std::vector<Measurement> m)
	{
		for (unsigned i = 0; i < dimension; ++i) {
			updateStep(m[i]);
		}
	}

	void updateStep(Measurement m)
	{
		incorporateMeasurement(m);
	}

	// getters
	const float getFilteredElement(int index)
	{
		return currentState(index);
	}

protected:
	virtual void incorporateMeasurement(Measurement m) = 0;

	// apply exponential formula (see @brief)
	const void updateDimension(int index, float observation)
	{
		const float current = getFilteredElement(index);
		const float withObservation = current * alpha + (1-alpha) * observation;

		currentState(index) = withObservation;
	}

private:
	// called by constructors
	void zeroCurrentState() {
		for (unsigned i = 0; i < dimension; i++) {
			currentState(i) = 0;
		}
	}
};

#endif // ExponentialFilter_h_DEFINED
