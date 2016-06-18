/*
Copyright 2013 The University of New South Wales (UNSW).

This file is part of the 2010 team rUNSWift RoboCup entry. You may
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

#include "Optimiser.h"
#include <vector>

/**
 * Implementation of the Nelder Mead (Simplex-Downhill) optimisation algorithm.
 * http://en.wikipedia.org/wiki/Nelder-Mead_method
 */
class NelderMead : public Optimiser {
public:
   std::vector<float> optimise(OptimisableFunction *target, unsigned num_params,
         unsigned max_evals);

private:

   struct SimplexVertex {
      std::vector<float> coord;
      float value;
   };

   struct SVComp {
      bool operator()(const SimplexVertex &v1, const SimplexVertex &v2){
         return v1.value < v2.value;
      }
   };

   unsigned num_params;
   std::vector<SimplexVertex> simplex_vertex; // all of the vertices of the simpex. should be n+1 vertices

   std::vector<float> centreOfMass(const std::vector<SimplexVertex> &vertices, unsigned num_vertices);

   std::vector<SimplexVertex> createInitialSimplex(OptimisableFunction *target, unsigned dim);

   SimplexVertex reflectedVertex(OptimisableFunction *target, const SimplexVertex &vertex,
         const std::vector<float> &rpoint, float reflect_dist=1.0f);

   SimplexVertex contractedVertex(OptimisableFunction *target, const SimplexVertex &vertex,
         const std::vector<float> &cpoint, float contract_ratio = 0.5f);
};
