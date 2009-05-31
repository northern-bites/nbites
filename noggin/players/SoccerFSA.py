# Soccer FSA that implements an FSA but holds all the important
# soccer-playing functionality
#
#

import man.motion as motion
from man.motion import HeadMoves
from ..util import FSA
from . import CoreSoccerStates

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

    def run(self):
        FSA.FSA.run(self)

    def executeMove(self,sweetMove):
        """
        Method to enqueue a SweetMove
        Can either take in a head move or a body command
        (see SweetMove files for descriptions of command tuples)
        """

        for position in sweetMove:
            if len(position) == 7:
                move = motion.BodyJointCommand(position[4], #time
                                               position[0], #larm
                                               position[1], #lleg
                                               position[2], #rleg
                                               position[3], #rarm
                                               position[6], # Chain Stiffnesses
                                               position[5], #interpolation type
                                               )


            elif len(position) == 4:
                move = motion.HeadJointCommand(position[1] ,# time
                                               position[0], # head pos
                                               position[3], # chain stiffnesses
                                               position[2], # interpolation type
                                                   )

            elif len(position) == 5:
                move = motion.BodyJointCommand(position[2], # time
                                               position[0], # chainID
                                               position[1], # chain angles
                                               position[4], # chain stiffnesses
                                               position[3], # interpolation type
                                               )

            else:
                self.printf("What kind of sweet ass-Move is this?")

            self.brain.motion.enqueue(move)
    def setSpeed(self,x,y,theta):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        if x == 0 and y == 0 and theta == 0:
            self.stopWalking()
        else:
            if self.brain.nav.setWalk(x,y,theta):
                self.brain.nav.switchTo('walking')
            else:
                if False: self.printf("WARNING NEW WALK of %g,%g,%g" % (x,y,theta) + " is ignored")

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
        if (self.brain.nav.currentState == 'stopped' or
            self.brain.nav.currentState == 'stop'):
            return
        else:
            self.brain.nav.setWalk(0,0,0)
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
        freeze = motion.FreezeCommand()
        self.brain.motion.sendFreezeCommand(freeze)

    def gainsOn(self):
        """
        Turn on the gains
        """
        unFreeze = motion.UnfreezeCommand(0.85)
        self.brain.motion.sendFreezeCommand(unFreeze)

    def penalizeHeads(self):
        """
        Put head into penalized position, stop tracker
        """
        self.brain.tracker.switchTo('stopped')
        self.brain.motion.stopHeadMoves()
        self.executeMove(HeadMoves.PENALIZED_HEADS)

    def zeroHeads(self):
        """
        Put heads into neutral position
        """
        self.brain.tracker.switchTo('stopped')
        self.brain.motion.stopHeadMoves()
        self.executeMove(HeadMoves.ZERO_HEADS)


    def walkPose(self):
        """
        We should usually not call this method from outside nav.
        This is a quasi-hack created by Tucker
        He can do hacks like this, you can't
        """
        self.brain.nav.setSpeed(0,0,0)
