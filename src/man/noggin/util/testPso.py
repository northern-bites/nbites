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

# testPso.py
# A series of unit tests of PSO function (Particle and Swarm)
# @author Nathan Merritt
# @date April 2011

import PSO as PSO
from MyMath import distance3d

#@todo set up ladder function test case as in:
# http://users.softlab.ece.ntua.gr/~ttsiod/ladders.html

# a simple 3d geometric search example
# the heuristic for each particle is the inverse of distance from DESTINATION

DESTINATION = (4, 3, 4)
geometric_mins = (0, 0, 0)
geometric_maxs = (50, 50, 50)
geometric_dimensions = 3
geometric_particles = 20

geometric_search = PSO.Swarm(geometric_particles, geometric_dimensions,
                             geometric_mins, geometric_maxs)
thisIteration = 0

print "Looking for point (4, 3, 4)"

while (geometric_search.getIterations() < 100):
    currentParticle = geometric_search.getCurrentParticle()
    particleLocation = currentParticle.getPosition()
    distanceFromTarget = distance3d(DESTINATION, particleLocation)

    currentParticle.setHeuristic(-distanceFromTarget) # since heuristic is maximized

    #print "set heuristic "
    #print -distanceFromTarget
    #print "\n"

    geometric_search.tickCurrentParticle()

    if geometric_search.getIterations() > thisIteration:
        thisIteration = geometric_search.getIterations()

        # check the best found "solution" after each 10 iterations
        if thisIteration % 10 == 0:
            (best_loc, best_score) = geometric_search.getBestSolution()
            print "Best solution after " + str(thisIteration) + " is: " \
                + str(best_loc) + " with score " + str(best_score)
