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

#include "TrainingSet.hpp"

/**
 * These represent the buckets to place training data into for the gradients.
 * E.g. any training sets with gradient between 0 and 1.5 would go in the first
 * bucket, 1.5 - 3 the second, etc.
 */
const double gradientBucketRangeValues[] = {0, 1.5, 3, 4.5, 6, 7.5, 9, 10.5, 12, 13.5, 15};
const std::vector<double> TrainingSet::GRADIENT_BUCKET_RANGES(gradientBucketRangeValues, gradientBucketRangeValues + sizeof(gradientBucketRangeValues) / sizeof(*gradientBucketRangeValues));

TrainingSet::TrainingSet() {
	intervals[WHITE] = COLOUR_BUCKET_INTERVAL;
	intervals[RED] = COLOUR_BUCKET_INTERVAL;
	intervals[BLUE] = COLOUR_BUCKET_INTERVAL;
	intervals[SONAR] = SONAR_BUCKET_INTERVAL;

	buckets[GRADIENT].assign(GRADIENT_BUCKET_RANGES.size(), 0);
	buckets[WHITE].assign(NUM_COLOUR_BUCKETS, 0);
	buckets[RED].assign(NUM_COLOUR_BUCKETS, 0);
	buckets[BLUE].assign(NUM_COLOUR_BUCKETS, 0);
	buckets[SONAR].assign(NUM_SONAR_BUCKETS + 1, 0);

	count = 0;
}

unsigned int TrainingSet::getCount() const {
	return count;
}

unsigned int TrainingSet::findGradientBucket(const double gradient) const {
	unsigned int bucket = GRADIENT_BUCKET_RANGES.size() - 1;
	for (unsigned int i = 0; i + 1 < GRADIENT_BUCKET_RANGES.size(); ++i) {
		if (GRADIENT_BUCKET_RANGES[i] <= gradient &&
			gradient < GRADIENT_BUCKET_RANGES[i + 1]) {
			bucket = i;
		}
	}
	return bucket;
}

unsigned int TrainingSet::findBucket(const double percentage, const unsigned int interval) const {
	unsigned int bucketIndex = percentage * 100 / interval;
	return bucketIndex;
}

unsigned int TrainingSet::findSonarBucket(const double percentage) const {
	if (percentage < 0) {
		return NUM_SONAR_BUCKETS;
	} else {
		return findBucket(percentage, intervals.at(SONAR));
	}
}


void TrainingSet::addTrainingExample(const double gradient, const double white, const double red, const double blue, const double sonar) {

	++buckets[GRADIENT][findGradientBucket(gradient)];
	++buckets[WHITE][findBucket(white, intervals[WHITE])];
	++buckets[RED][findBucket(red, intervals[RED])];
	++buckets[BLUE][findBucket(blue, intervals[BLUE])];
	++buckets[SONAR][findSonarBucket(sonar)];
	++count;
}

double TrainingSet::getProbabilityForExample(Bucket type, const double value, unsigned int laplaceEstimate) const {
	if (type == GRADIENT) {
		unsigned int bucketIndex = findGradientBucket(value);
		return ((double)buckets.at(type)[bucketIndex] + laplaceEstimate) / ((double)count + 2 * laplaceEstimate);
	} else if (type == WHITE || type == RED || type == BLUE) {
		unsigned int bucketIndex = findBucket(value, intervals.at(type));
		return ((double)buckets.at(type)[bucketIndex] + laplaceEstimate) / ((double)count + 2 * laplaceEstimate);
	} else if (type == SONAR) {
		unsigned int bucketIndex = findBucket(value, intervals.at(type));
		return ((double)buckets.at(type)[bucketIndex] + laplaceEstimate) / ((double)count + 2 * laplaceEstimate);
	}
	return 0;
}



