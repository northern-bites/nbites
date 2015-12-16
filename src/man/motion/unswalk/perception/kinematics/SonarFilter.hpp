#pragma once

#include <vector>
#include <utility>
#include <list>
#include "utils/body.hpp"
#include "motion/SonarRecorder.hpp"


#define MERGE_CUTOFF 50 // in mm, how close range observations must be to merge
#define CONFIDENCE_CUTOFF 3 // number of observations that must contain the obstacle
                            // (compare to WINDOW_SIZE in SonarRecorder)

/* Simple non-recursive clustering based filter for noisy sonar observations */
class SonarFilter {
   public:

      SonarFilter();

      /* Update the filtered observations */
      void update(   std::vector< std::vector <int> > sonarWindow );

      // Observations of multiple obstacles, each pair constains distance in m, confidence (0-1)
      std::vector< std::vector <int> > sonarFiltered; 


   private:

      /* Cluster to a particular cutoff */
      std::vector<int> cluster(std::vector<int> &sonar, int merge_cutoff, int confidence_cutoff);


};
