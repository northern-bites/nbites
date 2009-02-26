# Soccer FSA that implements an FSA but holds all the important
# soccer-playing functionality
#
# @author Jack Morrison
#

import math

from ..util import FSA
from . import GameControllerStates

class SoccerFSA(FSA.FSA):
    def __init__(self,brain):
        FSA.FSA.__init__(self, brain)
        #self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(GameControllerStates)


        # Method to enqueue a SweetMove
        # Can either take in a head move or a body command
        # (see SweetMove files for descriptions of command tuples)
        def executeMove(self,sweetMove):
            for position in sweetMove:
                if len(position) == 6:
                    move = motion.BodyJointCommand(position[4], #time
                                                   position[0], #larm
                                                   position[1], #lleg
                                                   position[2], #rleg
                                                   position[3], #rarm
                                                   position[5], #interpolation type
                                                   )
                    self.brain.motion.enqueue(move)

                elif len(position) == 3:
                    move = motion.HeadJointCommand(position[1],#time
                                                   position[0],#head pos
                                                   position[2],#interpolation type
                                                   )
                    self.brain.motion.enqueue(move)

                else:
                    print "What kind of sweet ass-Move is this?"

