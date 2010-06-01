# This file is part of Man, a robotic perception, locomotion, and
# team strategy application created by the Northern Bites RoboCup
# team of Bowdoin College in Brunswick, Maine, for the Aldebaran
# Nao robot.
#
# Man is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Man is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser Public License for more details.
#
# You should have received a copy of the GNU General Public License
# and the GNU Lesser Public License along with Man.  If not, see
# <http:#www.gnu.org/licenses/>.


# PSO.py: an implementation of a Particle-Swarm Optimizer
#       will search through an N-dimensional space

import random
import math

DEBUG = False

# how much we trend towards pBest, gBest (cog/soc biases)
# both set to ~2 per the PSO wikipedia article
COG = 0.7
SOC = 1.4

# particle motion may not exceed abs(V_CAP) in any tick
VELOCITY_CAP = 2

class Particle:
    def __init__(self, nSpace, searchMins, searchMaxs):
        self.dimension = nSpace

        self.pBest = 0
        self.pBest_vars = [0]*nSpace

        self.gBest = 0
        self.gBest_vars = []

        self.searchMins = searchMins
        self.searchMaxs = searchMaxs

        # our positions and velocities in N-space
        self.curr_position = [0]*nSpace
        self.curr_velocity = [0]*nSpace

        # inertia constant for velocity updates
        # initialized randomly per-particle as suggested by PSO wikipedia article
        self.INERTIAL = random.random()

        self.stability = 0

    def tick(self, gBest, gBest_vars):
        # wait for the motion system to try gait parameters
        # self.stability = self.getStability()

        # Update local best and its fitness
        if self.stability > self.pBest:
            self.pBest = self.stability
            self.pBest_vars = self.getPosition()

        # Update the global best and its fitness
        self.gBest = gBest
        self.gBest_vars = gBest_vars

        if self.stability > self.gBest:
            self.gBest = self.stability
            self.gBest_vars = self.getPosition()

        # Update the particle velocity :
        # Random component to avoid local minima
        self.R1 = random.random()
        self.R2 = random.random()

        # Apply velocities to positions
        # make sure to stay in bound of (searchMins[i], searchMaxs[i])
        for i in range(0, self.dimension):
            # if mins[i] == maxs[i] then ignore, it's a placeholder variable
            if self.searchMins[i] == self.searchMaxs[i]:
                continue

            self.curr_velocity[i] = self.INERTIAL*self.curr_velocity[i] \
                + self.R1*COG*(self.pBest_vars[i] - self.curr_position[i]) \
                + self.R2*SOC*(self.gBest_vars[i] - self.curr_position[i])

            if self.curr_velocity[i] > VELOCITY_CAP:
                self.curr_velocity[i] = VELOCITY_CAP
            elif self.curr_velocity[i] < -VELOCITY_CAP:
                self.curr_velocity[i] = -VELOCITY_CAP

            new_position = self.curr_position[i] + self.curr_velocity[i]

            if new_position > self.searchMaxs[i]:
                new_position = self.searchMaxs[i]
            elif new_position < self.searchMins[i]:
                new_position = self.searchMins[i]

            self.curr_position[i] = new_position

        # pass our gBest, gBest_vars back to the swarm controller
        return (self.gBest, self.gBest_vars)

    # used to decide how stable our most recent set of parameters were
    def getStability(self):
        # return distance from 5,5 (for testing)
        #return math.sqrt((self.curr_position[0] - 5)**2 + (self.curr_position[1] - 5)**2)

        return 0

    # when we set stability from outside the swarm, use this
    def setStability(self, outside_stability):
        self.stability = outside_stability
        return

    def getPosition(self):
        return self.curr_position

    # debug output for a particle
    def printState(self):
        print "pBest: %s gBest: %s nSpace: %s" % (self.pBest, self.gBest, self.dimension)
        print "Positions in N-Space: %s" % self.curr_position
        print "Velocity: %s" % self.curr_velocity

class Swarm:
    def __init__(self, numParticles, nSpace, searchMins, searchMaxs):
        self.particles = []
        self.partIndex = 0
        self.numParticles = numParticles

        self.gBest = 0
        self.gBest_vars = [0]*nSpace

        for i in range(0, numParticles):
            self.particles.append(Particle(nSpace, searchMins, searchMaxs))
            # uniformly distribute particles across search space and randomize velocity
            for j in range(0, nSpace):
                # don't optimize this parameter
                if searchMins[j] == searchMaxs[j]:
                    self.particles[i].curr_position[j] = searchMins[j]
                    self.particles[i].curr_velocity[j] = 0
                    continue

                self.particles[i].curr_position[j] = random.uniform(searchMins[j], \
                                                                    searchMaxs[j])
                self.particles[i].curr_velocity[j] = random.uniform(-VELOCITY_CAP*.5, \
                                                                     VELOCITY_CAP*.5)
        if DEBUG:
            for p in self.particles:
                p.printState()

    # probably not useful in actual RoboCup situations...
    def solve_swarm(self, iterations):
        for i in range(0, iterations):
            for p in self.particles:
                (self.gBest, self.gBest_vars) = p.tick(self.gBest, self.gBest_vars)

                if DEBUG:
                    p.printState()

    def getCurrParticle(self):
        return self.particles[self.partIndex]

    def getNextParticle(self):
        self.partIndex += 1

        if self.partIndex < self.numParticles:
            return self.particles[self.partIndex]
        else:
            self.partIndex = 0
            return self.particles[0]

    def tickCurrParticle(self):
        (self.gBest, self.gBest_vars) = \
               self.particles[self.partIndex].tick(self.gBest, self.gBest_vars)

        if DEBUG:
            self.particles[self.partIndex].printState()

    def saveSwarm(output_file):
        return

    def loadSwarm(input_file):
        return
