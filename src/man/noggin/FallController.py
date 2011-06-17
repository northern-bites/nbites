from man.motion import SweetMoves as SweetMoves
from . import FallStates
from .util import FSA

class FallController(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(FallStates)
        self.currentState = 'notFallen'
        self.setName('FallController')
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'blue'
        self.setPrintFunction(self.brain.out.printf)

        self.standingUp = False
        self.fallCount = 0
        self.doneStandingCount = 0
        self.standupMoveTime = 0

        self.DONE_STANDING_THRESH = 2

        self.executeStandup = True
        self.enabled = True

    def run(self):
        if not self.enabled:
            return

        # Only try to stand up when playing or localizing in ready
        if (self.brain.gameController.currentState == 'gamePlaying' or
            self.brain.gameController.currentState == 'gameReady' ):
            # Check to see if fallen over
            inertial = self.brain.sensors.inertial
            #self.printf("run angleY is "+str(inertial.angleY))

            if (not self.standingUp and self.brain.roboguardian.isRobotFallen() ):
                self.standingUp = True
                self.fallCount = 0
                self.switchTo('fallen')
                #         elif self.brain.guardian.falling:
                #             self.switchTo('falling')

            # if not falling, check if our feet are on the ground
            elif (not self.standingUp and
                  not self.brain.nav.doingSweetMove and
                  not self.brain.roboguardian.isFeetOnGround()):
                self.switchTo('feetOffGround')

        FSA.FSA.run(self)

    def getTimeRemainingEst(self):
        if (self.currentState == "notFallen" or
            self.currentState == "doneStanding"):
            return 0
        else:
            return SweetMoves.getMoveTime(SweetMoves.STAND_UP_FRONT)

    def enableFallProtection(self, isTrue):
        self.printf("Fall Protection is " + str(isTrue))
        self.enabled = isTrue
        self.brain.roboguardian.enableFallProtection(isTrue)

