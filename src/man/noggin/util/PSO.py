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
# @author Nathan Merritt

import random
from math import fabs
from MyMath import clip

DEBUG = True
DEBUG_POSITION = False
DEBUG_PROGRESS = True

# how much we trend towards pBest, gBest (cog/soc biases)
# both set to ~2 per the PSO wikipedia article
COG = 0.7
SOC = 1.4

# particle motion may not exceed abs(V_CAP) in any tick
VELOCITY_CAP = 1
VELOCITY_MINIMUM_MAGNITUDE = 0.01

INFINITY = float(1e3000)

class Particle:
    def __init__(self, nSpace, searchMins, searchMaxs):
        self.dimension = nSpace

        self.pBest = -INFINITY
        self.pBest_vars = [0]*nSpace

        self.gBest = -INFINITY
        self.gBest_position = []

        self.searchMins = searchMins
        self.searchMaxs = searchMaxs

        self.position = [0]*nSpace
        self.velocity = [0]*nSpace

        # inertia constant for velocity updates
        # initialized randomly per-particle as suggested by PSO wikipedia article
        self.INERTIAL = random.random()

        self.heuristic = 0
        self.moves = 0

    def tick(self, gBest, gBest_position):
        # Update local best and its fitness
        if self.heuristic > self.pBest:
            self.pBest = self.heuristic
            self.pBest_vars = self.getPosition()

        # Update the global best and its fitness
        self.gBest = gBest
        self.gBest_position = gBest_position

        if self.heuristic > self.gBest:
            self.gBest = self.heuristic
            self.gBest_position = self.getPosition()

        self.updateParticleVelocity()
        self.updateParticlePosition()

        self.moves += 1

        # pass our gBest, gBest_position back to the swarm controller
        return (self.gBest, self.gBest_position)

    def updateParticleVelocity(self):
        # Random component to avoid local minima
        R1 = random.random()
        R2 = random.random()

        for i in range(0, self.dimension):
            # if mins[i] == maxs[i] then ignore, we aren't optimizing it
            if self.searchMins[i] == self.searchMaxs[i]:
                continue

            self.velocity[i] = self.INERTIAL*self.velocity[i] \
                + R1*COG*(self.pBest_vars[i] - self.position[i]) \
                + R2*SOC*(self.gBest_position[i] - self.position[i])

            clip(self.velocity[i],
                 -VELOCITY_CAP,
                 VELOCITY_CAP)

            if fabs(self.velocity[i]) < VELOCITY_MINIMUM_MAGNITUDE:
                self.velocity[i] = 0


    def updateParticlePosition(self):
        for i in range(0, self.dimension):
            self.position[i] = self.position[i] + self.velocity[i]

            clip(self.position[i],
                 self.searchMins[i],
                 self.searchMins[i])

    # used to decide how stable our most recent set of parameters were
    def getHeuristic(self):
        return

    # when we set heuristic from outside the swarm, use this
    def setHeuristic(self, outside_heuristic):
        self.heuristic = outside_heuristic
        return

    def getPosition(self):
        return self.position

    # debug output for a particle
    def printState(self):
        if DEBUG_PROGRESS:
            print "pBest: %s gBest: %s nSpace: %s" % (self.pBest, self.gBest, self.dimension)
            print "particle has moved %s times" % self.moves
        if DEBUG_POSITION:
            print "Positions in N-Space: %s" % self.position
            print "Velocity: %s" % self.velocity

class Swarm:
    def __init__(self, numParticles, nSpace, searchMins, searchMaxs):
        self.particles = []
        self.partIndex = 0
        self.numParticles = numParticles

        self.gBest = -INFINITY
        self.gBest_position = [0]*nSpace

        self.iterations = 0 # how many times every particle has moved

        for i in range(0, numParticles):
            self.particles.append(Particle(nSpace, searchMins, searchMaxs))

            for j in range(0, nSpace):
                # don't optimize any parameter where min == max
                if searchMins[j] == searchMaxs[j]:
                    self.particles[i].position[j] = searchMins[j]
                    self.particles[i].velocity[j] = 0
                    continue

                # uniformly distribute particles across search space
                # and randomize initial velocity
                self.particles[i].position[j] = random.uniform(searchMins[j], \
                                                                    searchMaxs[j])
                self.particles[i].velocity[j] = random.uniform(-VELOCITY_CAP*.5, \
                                                                     VELOCITY_CAP*.5)
        if DEBUG:
            for p in self.particles:
                p.printState()

    # probably not useful in actual RoboCup situations...
    def solve_swarm(self, iterations):
        for i in range(0, iterations):
            for p in self.particles:
                (self.gBest, self.gBest_position) = p.tick(self.gBest, self.gBest_position)

                if DEBUG:
                    p.printState()

    def getIterations(self):
        return self.iterations

    def getCurrParticle(self):
        return self.particles[self.partIndex]

    def getNextParticle(self):
        self.partIndex += 1

        if self.partIndex < self.numParticles:
            return self.particles[self.partIndex]
        else:
            self.iterations += 1
            self.partIndex = 0
            return self.particles[0]

    def getBestSolution(self):
        return (self.gBest_position, self.gBest)

    def tickCurrParticle(self):
        (self.gBest, self.gBest_position) = \
               self.particles[self.partIndex].tick(self.gBest, self.gBest_position)

        if DEBUG:
            self.particles[self.partIndex].printState()

