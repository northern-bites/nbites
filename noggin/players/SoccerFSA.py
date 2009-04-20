# Soccer FSA that implements an FSA but holds all the important
# soccer-playing functionality
#
# @author Jack Morrison
#

import man.motion as motion

from ..util import FSA
from . import CoreSoccerStates

STANDUP_GAINS_VALUE = 1.0
GAINS_ON_VALUE = 0.85
GAINS_OFF_VALUE = 0.0

class SoccerFSA(FSA.FSA):
    def __init__(self,brain):
        FSA.FSA.__init__(self, brain)
        #self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(CoreSoccerStates)
        self.brain = brain
        self.motion = brain.motion
        #self.currentState = '' #handled externally by GameControllerFSA

        #set default behavior for soccer players - override it if you want
        self.setPrintStateChanges(True)
        # set printing to be done with colors
        self.stateChangeColor = 'red'
        self.setPrintFunction(self.brain.out.printf)

    def executeMove(self,sweetMove):
        """
        Method to enqueue a SweetMove
        Can either take in a head move or a body command
        (see SweetMove files for descriptions of command tuples)
        """
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
                self.printf("What kind of sweet ass-Move is this?")

    def setSpeed(self,x,y,theta):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        if x == 0 and y == 0 and theta == 0:
            if (self.brain.nav.currentState == 'stopped' or
                self.brain.nav.currentState == 'stop'):
                return
            else:
                self.brain.nav.switchTo('stop')
        else:
            if self.brain.nav.setWalk(x,y,theta):
                self.brain.nav.switchTo('walking')

    def standup(self):
        if self.brain.motion.isWalkActive():
            self.stopWalking()
        else:
            # TODO: this is not the best way to stand up
            dummyWalk = motion.WalkCommand(x=0,y=0,theta=0)
            self.brain.motion.setNextWalkCommand(dummyWalk)

    def stopWalking(self):
        """
        Wrapper method to navigator to easily stop the robot from walking
        """
        self.brain.nav.switchTo('stop')

    def setHeads(self,yawv,pitchv):
        """
        Wrapper method to easily specify a head destination (in degrees, obvi)
        """
        heads = motion.SetHeadCommand(yawv,pitchv)
        self.brain.motion.setHead(heads)

    def gainsOff(self):
        """
        Turn off the gains
        """
        shutoff = motion.StiffnessCommand(GAINS_OFF_VALUE)
        self.brain.motion.sendStiffness(shutoff)

    def gainsOn(self):
        """
        Turn on the gains
        """
        turnon = motion.StiffnessCommand(GAINS_ON_VALUE)
        self.brain.motion.sendStiffness(turnon)

    def standupGainsOn(self):
        """
        Turn on the gains
        """
        turnon = motion.StiffnessCommand(STANDUP_GAINS_VALUE)
        self.brain.motion.sendStiffness(turnon)
