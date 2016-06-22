#pragma once

#include <vector>

class OptimisableFunction {
public:
   virtual ~OptimisableFunction(){}

   /**
    * Returns the fitness of the given set of parameters.
    */
   virtual float eval(std::vector<float> &params) = 0;
};

/*
 * Abstract base class for optimisers.
 */
class Optimiser {
public:
   virtual ~Optimiser(){}

   /**
    * Find the optimal set of parameters for the given object function. Note that the parameters
    * should be in the range [0..1].
    */
   virtual std::vector<float> optimise(OptimisableFunction *target, unsigned num_params,
         unsigned max_evals) = 0;
};
