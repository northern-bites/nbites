#include "NelderMead.h"
#include "Optimiser.h"
#include "Common.h"
#include <algorithm>
#include <vector>
#include <cassert>
#include <iostream>

std::vector<float> NelderMead::optimise(OptimisableFunction *target, unsigned num_params,
      unsigned max_evals){

   std::vector<float> lowest_coord;
   float lowest_value = 0.0f;

   this->num_params = num_params;
   simplex_vertex = createInitialSimplex(target, num_params);

   SVComp comp;
   sort(simplex_vertex.begin(), simplex_vertex.end(), comp);

   for(unsigned iter = 0; iter < max_evals; iter++){
      // Find the centre of mass of the first n-1 vertices
      std::vector<float> centre_of_mass = centreOfMass(simplex_vertex, simplex_vertex.size() - 1);

      // reflect the worst vertex around the centre of mass of the rest of the vertices.
      NelderMead::SimplexVertex reflected_vertex =
            reflectedVertex(target, simplex_vertex.back(), centre_of_mass);

      // if this reflected vertex is the best vertex, try expanding it even further.
      if(reflected_vertex.value < simplex_vertex.front().value){
         NelderMead::SimplexVertex extended_vertex =
               reflectedVertex(target, simplex_vertex.back(), centre_of_mass, 2.0f);

         if(extended_vertex.value < reflected_vertex.value){
            simplex_vertex.back() = extended_vertex;
         }
         else{
            simplex_vertex.back() = reflected_vertex;
         }
      }

      // If the reflected vertex is decent then just replace the worst with it.
      else if(reflected_vertex.value <= simplex_vertex.at(simplex_vertex.size() - 2).value){
         simplex_vertex.back() = reflected_vertex;
      }

      // if the vertex we reflected is the worst, but still better than the one we started with, try
      // contracting the original worst.
      else if(reflected_vertex.value < simplex_vertex.back().value){
         simplex_vertex.back() = contractedVertex(target, simplex_vertex.back(), centre_of_mass);
      }

      // otherwise this reflected vertex is REALLY bad, even worse than the original, so contract the
      // whole simplex towards the best point.
      else{
         for(unsigned i = 1; i < simplex_vertex.size(); i++){
            simplex_vertex.at(i) = contractedVertex(target, simplex_vertex.at(i),
                  simplex_vertex.front().coord);
         }
      }

      sort(simplex_vertex.begin(), simplex_vertex.end(), comp);

      if(simplex_vertex.front().value < lowest_value || iter == 0){
         lowest_coord = simplex_vertex.front().coord;
         lowest_value = simplex_vertex.front().value;
      }
   }

   return lowest_coord;
}

std::vector<float> NelderMead::centreOfMass(const std::vector<NelderMead::SimplexVertex> &vertices,
      unsigned num_vertices){
   std::vector<float> result(num_params, 0.0f);
   float val_sum = 0.0f;

   for(unsigned i = 0; i < num_vertices; i++){
      assert(vertices.at(i).coord.size() == num_params);

      for(unsigned j = 0; j < vertices.at(i).coord.size(); j++){
         result.at(j) += vertices.at(i).coord.at(j) * vertices.at(i).value;
      }

      val_sum += vertices.at(i).value;
   }

   for(unsigned i = 0; i < result.size(); i++){
      result.at(i) /= val_sum;
   }

   return result;
}


std::vector<NelderMead::SimplexVertex> NelderMead::createInitialSimplex(
      OptimisableFunction *target, unsigned dim){

   std::vector<NelderMead::SimplexVertex> result;
   NelderMead::SimplexVertex prime_vertex;

   for(unsigned i = 0; i < dim; i++){
      prime_vertex.coord.push_back(0.5f);
   }

   result.push_back(prime_vertex);

   // The rest of the vertices are just a constant offset from the prime vertex in each
   // separate dimension.
   for(unsigned i = 0; i < dim; i++){
      NelderMead::SimplexVertex new_vertex = prime_vertex;
      new_vertex.coord.at(i) += 0.01f; // TODO define this
      result.push_back(new_vertex);
   }

   // Calculate the function values at each vertex.
   for(unsigned i = 0; i < result.size(); i++){
      result.at(i).value = target->eval(result.at(i).coord);
   }

   assert(result.size() == dim+1);
   return result;
}

NelderMead::SimplexVertex NelderMead::reflectedVertex(OptimisableFunction *target,
      const NelderMead::SimplexVertex &vertex, const std::vector<float> &rpoint,
      float reflect_dist){

   assert(rpoint.size() == vertex.coord.size());

   NelderMead::SimplexVertex result;
   for(unsigned i = 0; i < vertex.coord.size(); i++){
      result.coord.push_back(rpoint.at(i) + reflect_dist*(rpoint.at(i) - vertex.coord.at(i)));
   }

   assert(result.coord.size() == rpoint.size());
   result.value = target->eval(result.coord);
   return result;
}

NelderMead::SimplexVertex NelderMead::contractedVertex(OptimisableFunction *target, 
      const NelderMead::SimplexVertex &vertex, const std::vector<float> &cpoint,
      float contract_ratio){

   assert(cpoint.size() == vertex.coord.size());

   NelderMead::SimplexVertex result;
   for(unsigned i = 0; i < vertex.coord.size(); i++){
      result.coord.push_back(cpoint.at(i) + contract_ratio*(vertex.coord.at(i) - cpoint.at(i)));
   }

   assert(result.coord.size() == cpoint.size());
   result.value = target->eval(result.coord);
   return result;
}

