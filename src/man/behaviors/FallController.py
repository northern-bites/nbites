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

        # Only try to stand up when playing or localizing in ready because
        # it's really annoying otherwise.
        if ('Playing' in self.brain.gameController.currentState or
            'Ready' in self.brain.gameController.currentState):

            #self.printf("run angleY is "+str(inertial.angleY))

            if (not self.standingUp and self.brain.roboguardian.isRobotFallen() ):
                self.standingUp = True
                self.fallCount = 0
                self.switchTo('fallen')
                #         elif self.brain.guardian.falling:
                #             self.switchTo('falling')

            # disabled due to flakey FSRs (7/8/11)
            """
            elif (not self.standingUp and
                  not self.brain.nav.doingSweetMove and
                  not self.brain.roboguardian.isFeetOnGround()):
                self.switchTo('feetOffGround')
            """

        FSA.FSA.run(self)

    def getTimeRemainingEst(self):
        # TODO: turn this into a message
        return 0
        # if (self.currentState == "notFallen" or
        #     self.currentState == "doneStanding"):
        #     return 0
        # else:
        #     return SweetMoves.getMoveTime(SweetMoves.STAND_UP_FRONT)

    def enableFallProtection(self, isTrue):
        self.printf("Fall Protection is " + str(isTrue))
        self.enabled = isTrue
        self.brain.roboguardian.enableFallProtection(isTrue)
