
import man.motion as motion
import man.motion.SweetMoves as SweetMoves
from . import FallStates
from .util import FSA

import math

class FallController(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(FallStates)
        self.currentState = 'nothing'
        self.setName('FallController')
        self.setPrintStateChanges(True)
        self.stateChangeColor = 'blue'
        self.setPrintFunction(self.brain.out.printf)
        self.standingUp = False
        self.fallCount = 0
        self.FALLEN_THRESH = 72

    def run(self):
        # Check to see if fallen over is alerted by guardian
        if (self.isFallen() and
             not self.standingUp ):
            self.switchTo('fallen')
#         elif self.brain.guardian.falling:
#             self.switchTo('falling')

asg
        FSA.FSA.run(self)

    def isFallen(self):
        print "y is ",self.brain.sensors.inertial[6]
        if ( math.fabs(self.brain.sensors.inertial[6]) > self.FALLEN_THRESH ):
            self.fallCount += 1
            if self.fallCount > 5:
                return True
        else:
            self.fallCount = 0
            return False

