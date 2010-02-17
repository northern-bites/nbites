# Soccer FSA that implements an FSA but holds all the important
# soccer-playing functionality
#
#
from man.motion import HeadMoves
import man.motion as motion
from ..util import FSA
from . import CoreSoccerStates

class SoccerFSA(FSA.FSA):
    def __init__(self,brain):
        FSA.FSA.__init__(self, brain)
        #self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(CoreSoccerStates)
        self.brain = brain
        self.motion = brain.motion

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

    def setWalk(self,x,y,theta):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        if x == 0 and y == 0 and theta == 0:
            self.stopWalking()
        else:
            self.brain.nav.walk(x,y,theta)
            # else:
            #     self.printf("WARNING NEW WALK of %g,%g,%g" % (x,y,theta) +
            #                 " is ignored")

    def setSteps(self, x, y, theta, numSteps=1):
        """
        Have the robot walk a specified number of steps
        """
        if self.brain.motion.isWalkActive():
            return False
        else:
            self.brain.nav.takeSteps(x, y, theta, numSteps)
            return True

    def standup(self):
        self.stopWalking()

    def walkPose(self):
        """
        we return to std walk pose when we stop walking
        """
        self.stopWalking()

    def stopWalking(self):
        """
        Wrapper method to navigator to easily stop the robot from walking
        """
        self.brain.nav.stop()

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
        self.brain.tracker.performHeadMove(HeadMoves.PENALIZED_HEADS)

    def zeroHeads(self):
        """
        Put heads into neutral position
        """
        self.brain.tracker.performHeadMove(HeadMoves.ZERO_HEADS)
