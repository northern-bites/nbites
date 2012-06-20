# Soccer FSA that implements an FSA but holds all the important
# soccer-playing functionality
#
#
from man.motion import HeadMoves
import man.motion as motion
from ..util import FSA
from . import CoreSoccerStates

class SoccerFSA(FSA.FSA):
    def __init__(self, brain):
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

        # for writing variance data only once per appropriate state
        self.wroteVarianceData = False

        # stuff so players don't crash.
        self.inKickingState = False
        self.saveBallPosition()

        # Penalty kick player variables
        self.penaltyKicking = False

        # Kickoff kick
        self.hasKickedOff = True

    def run(self):
        # gamePenalized is a good time for a lot of i/o, since we
        # won't be moving
        if self.currentState == 'gamePenalized' or \
               self.currentState == 'gameFinished':
            if not self.wroteVarianceData:
                self.brain.sensors.writeVarianceData()
                self.wroteVarianceData = True
        else:
            self.wroteVarianceData = False

        FSA.FSA.run(self)

    def executeMove(self, sweetMove):
        """
        Method to do a SweetMove from the behavior
        Can either take in a head move or a body command
        (see SweetMove files for descriptions of command tuples)
        """
        self.brain.nav.performSweetMove(sweetMove)

    def setWalk(self, x, y, theta):
        """
        Wrapper method to easily change the walk vector of the robot
        """
        if x == 0 and y == 0 and theta == 0:
            self.stand()
        else:
            self.brain.nav.walk(x, y, theta)

    def stand(self):
        """ Set the navigator/motion engine to stand"""
        self.brain.nav.stand()

    def stopWalking(self):
        """
        Wrapper method to navigator to easily stop the robot from walking
        """
        self.stand()

    def ballMoved(self):
        """
        Returns true if the ball has moved in the absolute frame since the last time
        saveBallPosition() was called, or if the uncertainty is too high
        """
        if abs(self.brain.ball.loc.x - self.lastBall_x) > 20 or \
               abs(self.brain.ball.loc.y - self.lastBall_y) > 20:
            print "Ball has moved globally since we last saved"
            self.brain.speech.say("Ball moved")
            return True

        return False

    def saveBallPosition(self):
        self.lastBall_x = self.brain.ball.loc.x
        self.lastBall_y = self.brain.ball.loc.y

##### Direct Motion Calls
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

##### HEAD-TRACKING Methods
    def penalizeHeads(self):
        """
        Put head into penalized position, stop tracker
        """
        self.brain.tracker.penalizeHeads()

    def zeroHeads(self):
        """
        Put heads into neutral position
        """
        self.brain.tracker.performHeadMove(HeadMoves.ZERO_HEADS)

    def kickScan(self):
        self.brain.tracker.performHeadMove(HeadMoves.KICK_SCAN)
