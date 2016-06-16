/*
Copyright 2014 The University of New South Wales (UNSW).

This file is part of the 2014 team rUNSWift RoboCup entry. You may
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version as
modified below. As the original licensors, we add the following
conditions to that license:

In paragraph 2.b), the phrase "distribute or publish" should be
interpreted to include entry into a competition, and hence the source
of any derived work entered into a competition must be made available
to all parties involved in that competition under the terms of this
license.

In addition, if the authors of a derived work publish any conference
proceedings, journal articles or other academic papers describing that
derived work, then appropriate academic citations to the original work
must be included in that publication.

This rUNSWift source is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this source code; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include <vector>
#include <map>

class TrainingSet {
public:
	/**
	 * The bucket represents a set of training data. For example, the Gradient bucket
	 * would represent all the gradient training data.
	 */
	enum Bucket {
		GRADIENT,
		WHITE,
		RED,
		BLUE,
		SONAR
	};

	TrainingSet();

	void addTrainingExample(const double gradient, const double white, const double red, const double blue, const double sonar);
	double getProbabilityForExample(const Bucket type, const double colourPercentage, const unsigned int laplaceEstimate) const;

	unsigned int getCount() const;

private:

	const static std::vector<double> GRADIENT_BUCKET_RANGES;

	const static unsigned int COLOUR_BUCKET_INTERVAL = 10;
	const static unsigned int NUM_COLOUR_BUCKETS = 100 / COLOUR_BUCKET_INTERVAL;

	const static unsigned int SONAR_BUCKET_INTERVAL = 10;
	const static unsigned int NUM_SONAR_BUCKETS = 100 / SONAR_BUCKET_INTERVAL;

	std::map<Bucket, std::vector<int> > buckets;
	std::map<Bucket, unsigned int> intervals;

	unsigned int count;

	unsigned int findBucket(const double percentage, const unsigned int interval) const;
	unsigned int findGradientBucket(const double gradient) const;
	unsigned int findSonarBucket(const double percentage) const;
};
