#include "ParticleSwarm.h"
#include "Common.h"
#include <vector>
#include <cassert>
#include <iostream>

ParticleSwarm::ParticleSwarm(unsigned num_particles) : num_particles(num_particles) {}

std::vector<float> ParticleSwarm::optimise(OptimisableFunction *target, unsigned np, 
      unsigned max_evals){

   num_params = np;
   omega = 0.95f;
   c1 = 0.3f;
   c2 = 0.3f;

   // Initialise the swarm.
   for(unsigned i = 0; i < num_particles; i++){
      swarm.push_back(createInitialParticle(target));
      if(swarm.back().best_known_value < best_global_value || i == 0){
         best_global_value = swarm.back().best_known_value;
         best_global_position = swarm.back().best_known_position;
      }
   }

   float prev_best = 0.0f;
   for(unsigned iter = 0; iter < max_evals; iter++){
      if(iter == 0 || best_global_value < prev_best){
         prev_best = best_global_value;
      }
      updateParticles(target);
   }

   return best_global_position;
}

void ParticleSwarm::updateParticles(OptimisableFunction *target){
   for(unsigned i = 0; i < swarm.size(); i++){
      std::vector<float> r1 = createRandomVector(num_params, 0.0f, 1.0f);
      std::vector<float> r2 = createRandomVector(num_params, 0.0f, 1.0f);

      // update the particles position.
      swarm.at(i).position = componentAdd(swarm.at(i).position, swarm.at(i).velocity);

      // Calculate the vectors from the particle position to the local and global best.
      for(unsigned j = 0; j < swarm.at(i).velocity.size(); j++){
         swarm.at(i).velocity.at(j) = omega*swarm.at(i).velocity.at(j) +
               c1*r1.at(j)*(swarm.at(i).best_known_position.at(j) - swarm.at(i).position.at(j)) +
               c2*r2.at(j)*(best_global_position.at(j) - swarm.at(i).position.at(j));
      }

      float current_particle_value = target->eval(swarm.at(i).position);
      if(current_particle_value < swarm.at(i).best_known_value){
         swarm.at(i).best_known_value = current_particle_value;
         swarm.at(i).best_known_position = swarm.at(i).position;

         if(current_particle_value < best_global_value){
            best_global_value = current_particle_value;
            best_global_position = swarm.at(i).position;
         }
      }
   }
}

std::vector<float> ParticleSwarm::createRandomVector(unsigned length, float min, float max){
   std::vector<float> result;
   for(unsigned i = 0; i < length; i++){
      result.push_back(rand_range(min, max));
   }
   return result;
}

std::vector<float> ParticleSwarm::componentMultiply(const std::vector<float> &var1,
      const std::vector<float> &var2){
   assert(var1.size() == var2.size());
   std::vector<float> result;
   for(unsigned i = 0; i < var1.size(); i++){
      result.push_back(var1.at(i) * var2.at(i));
   }
   return result;
}

std::vector<float> ParticleSwarm::scalarMultiply(float scal,
      const std::vector<float> &var){
   std::vector<float> result;
   for(unsigned i = 0; i < var.size(); i++){
      result.push_back(scal * var.at(i));
   }
   return result;
}

std::vector<float> ParticleSwarm::componentAdd(const std::vector<float> &var1,
      const std::vector<float> &var2){
   assert(var1.size() == var2.size());
   std::vector<float> result;
   for(unsigned i = 0; i < var1.size(); i++){
      result.push_back(var1.at(i) + var2.at(i));
   }
   return result;
}

ParticleSwarm::Particle ParticleSwarm::createInitialParticle(OptimisableFunction *target){
   ParticleSwarm::Particle np;
   np.position = createRandomVector(num_params, -1.0f, 1.0f);
   np.velocity = std::vector<float>(num_params, 0.0f);

   np.best_known_position = np.position;
   np.best_known_value = target->eval(np.position);

   return np;
}
