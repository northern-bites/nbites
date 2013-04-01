#!/usr/bin/python
#
# Extracts the best found gait from a PSO Swarm pickle instance
#
# @author Nathan Merritt
# @date May 2011 

import PSO as PSO
import GaitOptimizeHelpers
import sys
from os.path import isfile

try:
   import cPickle as pickle
except:
   import pickle

USAGE = "Usage: ExtractGait.py <swarm pickle file>"

if not len(sys.argv) > 1:
    print USAGE
    sys.exit()

pickleFile = sys.argv[1]

if not isfile(pickleFile):
    print USAGE
    sys.exit()

destFile = sys.argv[2]

if isfile(destFile):
    print "destination file exists, exiting..."
    sys.exit()

try:
    swarm = pickle.load(pickleFile)
    (gait, score) = swarm.getBestSolution()

except:
    print "error loading swarm"

try:
    f = open(destFile, 'w')
    writeGaitToFile(f, gait, score)
    f.close()
    print "Sucessfully extracted best gait"

except:
    print "error writing gait"

