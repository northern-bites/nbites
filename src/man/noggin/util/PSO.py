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
from math import (fabs,
                  sqrt)
from MyMath import clip

DEBUG = True # shuts off all debug statements
DEBUG_POSITION = False
DEBUG_PROGRESS = False
DEBUG_REGROUPING = True

RANDOMIZE_LOST_PARTICLES = True

REGROUPING = True
# the % of the search space all particles must be within to regroup
REGROUP_THRESH = 1.1*10**(-4)
REGROUP_FACTOR = 6/(5*REGROUP_THRESH)

# how much we trend towards pBest, gBest (cog/soc biases)
# both set to <2 per the PSO wikipedia article
COG = 1.0
SOC = 0.7
MAX_INERTIAL = 0.5

MAX_VEL_PERCENT = 0.25 # particles can cover up to 1/4 the range in 1 tick

VELOCITY_MINIMUM_MAGNITUDE = 0.0001

INFINITY = float(1e3000)

class Particle:
    def __init__(self, nSpace, searchMins, searchMaxs):
        self.dimension = nSpace

        self.pBest = -INFINITY
        self.gBest = -INFINITY

        self.position = [0]*nSpace
        self.velocity = [0]*nSpace

        self.setBounds(searchMins, searchMaxs)

        # initialized randomly per-particle as suggested by PSO wikipedia article
        self.INERTIAL = MAX_INERTIAL * random.random()

        self.heuristic = 0
        self.moves = 0

        for j in range(0, self.dimension):
            # don't optimize any parameter where min == max
            if self.searchMins[j] == self.searchMaxs[j]:
                self.position[j] = self.searchMins[j]
                self.velocity[j] = 0
            else:
                self.position[j] = random.uniform(self.searchMins[j],
                                                  self.searchMaxs[j])
                self.velocity[j] = random.uniform(-self.velocityCap[j],
                                                   self.velocityCap[j])

        # disable cog/soc biases until we have real data
        self.pBest_position = self.getPosition()
        self.gBest_position = self.getPosition()

    def tick(self, gBest, gBest_position):
        # Update local best and its fitness
        if self.heuristic > self.pBest:
            self.pBest = self.heuristic
            self.pBest_position = self.getPosition()

        # Update the global best and its fitness
        self.gBest = gBest
        self.gBest_position = gBest_position

        self.updateParticleVelocity()
        self.updateParticlePosition()

        self.moves += 1

        # pass our gBest, gBest_position back to the swarm controller
        if self.pBest > gBest:
            return (self.pBest, self.pBest_position)
        else:
            return (self.gBest, self.gBest_position)

    def updateParticleVelocity(self):
        for i in range(0, self.dimension):
            # if mins[i] == maxs[i] then ignore, we aren't optimizing it
            if self.searchMins[i] == self.searchMaxs[i]:
                continue

            # Random component to avoid local minima
            R1 = random.random()
            R2 = random.random()

            newVelocity = self.INERTIAL*self.velocity[i] \
                + COG * R1 * (self.pBest_position[i] - self.position[i]) \
                + SOC * R2 * (self.gBest_position[i] - self.position[i])

            self.velocity[i] = clip(newVelocity,
                                    -self.velocityCap[i],
                                    self.velocityCap[i])

            if fabs(self.velocity[i]) < VELOCITY_MINIMUM_MAGNITUDE:
                self.velocity[i] = 0

    def updateParticlePosition(self):
        for i in range(0, self.dimension):
            newPosition = self.position[i] + self.velocity[i]

            if ((newPosition > self.searchMaxs[i]) or
                (newPosition < self.searchMins[i]) and
                RANDOMIZE_LOST_PARTICLES):
                print "Randomized a lost particle!"
                self.position[i] = random.uniform(self.searchMins[i],
                                                  self.searchMaxs[i])
                self.velocity[i] = 0
            else:
                self.position[i] = clip(newPosition,
                                        self.searchMins[i],
                                        self.searchMaxs[i])

    # used to decide how stable our most recent set of parameters were
    def getHeuristic(self):
        return

    # when we set heuristic from outside the swarm, use this
    def setHeuristic(self, outside_heuristic):
        self.heuristic = outside_heuristic
        return

    # DON'T CALL THIS PUBLICALLY
    def _setVelocityCap(self):
        self.velocityCap = [MAX_VEL_PERCENT*(sMax - sMin) for sMax, sMin in zip(self.searchMaxs, self.searchMins)]

    # for use after REGROUPING
    def setBounds(self, searchMins, searchMaxs):
        self.searchMins = searchMins
        self.searchMaxs = searchMaxs

        self._setVelocityCap()

    # for use after REGROUPING
    def setPosition(self, position):
        self.position = position

    def getPosition(self):
        return self.position

    def avgAbsVelocity(self):
        velocitySum = optimizeDimensions = 0
        i = -1

        for velocity in self.velocity:
            i += 1
            if self.searchMins[i] == self.searchMaxs[i]:
                continue

            optimizeDimensions += 1
            velocitySum += fabs(velocity)

        if optimizeDimensions == 0:
            return 0

        return velocitySum / optimizeDimensions

    def distanceNd(self, ptB):
        '''
        Unlike the MyMath version, this one skips terms where searchMaxs==searchMins
        This is necessary to make distance calculations with regard to regrouping work
        '''
        # add up all the terms of form (ptA[i] - ptB[i])^2
        dimensionality = min(self.dimension, len(ptB))
        differences_squared = 0

        for i in range(0, dimensionality):
            if self.searchMins[i] == self.searchMaxs[i]:
                continue

            differences_squared += (self.position[i] - ptB[i])**2

        return sqrt(differences_squared)

    # debug output for a particle
    def printState(self):
        if DEBUG_PROGRESS:
            print "pBest: %s gBest: %s nSpace: %s" % (self.pBest, self.gBest, self.dimension)
            print "  particle's average abs(velocity) is %s" % self.avgAbsVelocity()
            print "  particle has moved %s times" % self.moves
        if DEBUG_POSITION:
            print "  Positions in N-Space: %s" % self.position
            print "  Velocity: %s" % self.velocity

class Swarm:
    """
    Manages a list of particles, ticks them and keeps track of the global
    best solution found. If REGROUPING = True, will scatter particles after
    premature convergence on a local minimum
    """
    def __init__(self, numParticles, nSpace, searchMins, searchMaxs):
        self.particles = []
        self.partIndex = 0
        self.numParticles = numParticles

        self.gBest = -INFINITY
        self.gBest_position = [0]*nSpace

        self.new_gBest = -INFINITY
        self.new_gBest_position = [0]*nSpace

        self.currSearchMaxs = searchMaxs
        self.currSearchMins = searchMins

        self.initialSearchMins = searchMins
        self.initialSearchMaxs = searchMaxs

        self.nSpace = nSpace
        self.searchSpaceSize = self.calculateSearchSize()

        self.iterations = 0 # how many times every particle has moved

        for i in range(0, numParticles):
            self.particles.append(Particle(nSpace, searchMins, searchMaxs))

        for p in self.particles:
            if DEBUG:
                p.printState()

    # probably not useful in actual RoboCup situations...
    def solve_swarm(self, iterations):
        for i in range(0, iterations):
            for p in self.particles:
                (self.gBest, self.gBest_position) = \
                    p.tick(self.gBest, self.gBest_position)

                if DEBUG:
                    p.printState()

    def getIterations(self):
        return self.iterations

    def getCurrentParticle(self):
        return self.particles[self.partIndex]

    def getBestSolution(self):
        return (self.gBest_position, self.gBest)

    def regroupSwarm(self):
        '''
        Saves our current gBest & gBest_position,  and then
        randomizes particles around it, to allow for search to
        continue after the swarm has converged

        Steps involved:
        Calculate new range
         (for each dimension i)
          1) uncertainty[i] = calculate maximum deviation of a particle from gBest[i]
          2) range[i] = minimum of: initial range on i, uncertainty * regrouping factor
        Randomly distribute particles across new range, which is centered on gBest
           cut all particle's velocities in half in all dimensions
        Keep the previous gBest
        Recalculate velocity clamping
        Recalculate search space boundaries

        Swarm can now start calculating again. Particles will still be attracted to the
        previous gBest, but this is fine.

        See (Evers, G. 2009) or look up RegPSO for more details
        '''

        newMins = [0]*self.nSpace
        newMaxs = [0]*self.nSpace

        # calculate new range
        for i in range(0, self.nSpace):
            maximum_distance = 0 # uncertainty

            # which particle in this dimension is the furthest from gBest
            for p in self.particles:
                particleLoc = p.getPosition()
                distance = fabs(particleLoc[i] - self.gBest_position[i])

                if distance > maximum_distance:
                    maximum_distance = distance

            initial_range_i = self.initialSearchMaxs[i] - self.initialSearchMins[i]
            new_range_i = min(initial_range_i, maximum_distance*REGROUP_FACTOR)

            newMins[i] = self.gBest_position[i] - .5*new_range_i
            newMaxs[i] = self.gBest_position[i] + .5*new_range_i

        # randomly spread particles within the new range
        for p in self.particles:
            newPosition = [0]*self.nSpace

            for i in range(0, self.nSpace):
                R_i = random.random()
                range_i = newMaxs[i] - newMins[i]

                newPosition[i] = self.gBest_position[i] \
                    + R_i*range_i \
                    - 0.5*range_i

            if False:
                print ""
                print "old"
                print p.getPosition()
                print "new"
                print newPosition
                print ""

            p.setPosition(newPosition)
            p.setBounds(newMins, newMaxs)

        if True:
            print "mins"
            print self.initialSearchMins
            print newMins

            print "maxs"
            print self.initialSearchMaxs
            print newMaxs

        # update various swarm parameters
        self.currSearchMins = newMins
        self.currSearchMaxs = newMaxs
        self.searchSpaceSize = self.calculateSearchSize()

    def tickCurrentParticle(self):
        '''
        Tells the current particle the gBest, ticks it, listens to see if
        it found a potential new gBest
        Each time we tick the last particle in this iteration, run iterationUpkeep()
        '''
        (this_gBest, this_gBest_position) = \
            self.particles[self.partIndex].tick(self.gBest, self.gBest_position)

        if DEBUG and DEBUG_PROGRESS:
            self.particles[self.partIndex].printState()

        # save this particle's idea of gBest, if it's any good
        if this_gBest > self.new_gBest:
            self.new_gBest = this_gBest
            self.new_gBest_position = this_gBest_position

        # increment the particle index after we tick it
        self.partIndex += 1

        # if this is a new iteration, do some housekeeping
        if self.partIndex >= self.numParticles:
            self.iterationUpkeep()


    def iterationUpkeep(self):
        '''
        Updates swarm's gBest, gBest_position
        Checks for premature convergence of the Swarm, if so triggers
        particle regrouping. This helps to avoid local minima and premature
        convergence (search: RegPSO on the 'net)
        '''
        self.iterations += 1
        self.partIndex = 0

        # update Swarm's gBest, gBest_position
        if self.new_gBest > self.gBest:
            self.gBest = self.new_gBest
            self.gBest_position = self.new_gBest_position

        self.new_gBest = self.gBest

        if not REGROUPING:
            return

        # Check for premature convergence of the Swarm, as defined by all
        # particles being closer than a percentage of the search space to
        # the gBest_position
        furthestParticleDistance = 0;

        for p in self.particles:
            gBestDistance = p.distanceNd(self.gBest_position)
            if gBestDistance > furthestParticleDistance:
                furthestParticleDistance = gBestDistance

        if DEBUG and DEBUG_REGROUPING:
            print "furthestParticleDistance: %s" % furthestParticleDistance
            print "regroup threshold: %s" % (self.searchSpaceSize*REGROUP_THRESH)

        if furthestParticleDistance < self.searchSpaceSize * REGROUP_THRESH:
            if DEBUG:
                print "Prematurely converged! Regrouping around current gBest"
            self.regroupSwarm()

    def calculateSearchSize(self):
        '''
        The search space size is equal to:
        (searchMaxs[0]-searchMins[0])*...*(searchMaxs[i]-searchMins[i])
        for i on [0, dimension of the swarm]
        '''
        size = 1

        for i in range(0, self.nSpace):
            if self.currSearchMins[i] == self.currSearchMaxs[i]:
                continue

            size *= fabs(self.currSearchMaxs[i] - self.currSearchMins[i])

        if DEBUG and DEBUG_REGROUPING:
            print "search space size: %s" % size

        return size
