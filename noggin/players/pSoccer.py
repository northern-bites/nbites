
import man.motion as motion

from ..util import FSA
from . import NoneStates

class SoccerPlayer(FSA.FSA):
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        #jf- self.setTimeFunction(self.brain.nao.getSimulatedTime)
        self.addStates(NoneStates)
	self.currentState = 'nothing'
        self.setName('Player pNone')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)

    def executeMove(self,sweetMove):
        for position in sweetMove:
            if len(position) == 6:
                move = motion.BodyJointCommand(position[4], #time
                                               position[0], #larm
                                               position[1], #lleg
                                               position[2], #rleg
                                               position[3], #rarm
                                               position[5], #interpolation time
                                               )
                self.brain.motion.enqueue(move)

            elif len(position) == 3:
                move = motion.HeadJointCommand(position[1],#time
                                               position[0],#head pos
                                               position[2],#interpolation time
                                               )
                self.brain.motion.enqueue(move)

            else:
                print "What kind of sweet ass-Move is this?"

    def setWalkStraightConfig(self):
        self.brain.motion.setWalkExtraConfig(5.85,-5.85,0.19,5.0)
        self.brain.motion.setWalkConfig(.04,.015,.04,.3,.018,.025)

    def setWalkTurnConfig(self):
        self.brain.motion.setWalkExtraConfig(5.85,-5.85,0.19,5.0)
        self.brain.motion.setWalkConfig(.04,.015,.04,.1,.018,.025)

    def setWalkSidewaysConfig(self):
        self.brain.motion.setWalkExtraConfig(4.2,-4.2,0.19,5.0)
        self.brain.motion.setWalkConfig(.02,.015,.02,.3,.018,0.015)

    def shouldFindBall(self):
        return self.brain.ball.framesOff > 5

    def inlineWithGoal(self):
        shootingObjBearing = None

        if self.brain.oppGoalCrossbar.on:
            shootingObjBearing = self.brain.oppGoalCrossbar.bearing
        elif self.brain.oppGoalLeftPost.on and self.brain.oppGoalRightPost.on:
            shootingObjBearing = (self.brain.oppGoalRightPost.bearing +
                                  self.brain.oppGoalLeftPost.bearing)/2
        elif self.brain.oppGoalLeftPost.on:
            shootingObjBearing = (self.brain.oppGoalLeftPost.bearing -
                                  BEARING_FUDGE)
        elif self.brain.oppGoalRightPost.on:
            shootingObjBearing = (self.brain.oppGoalRightPost.bearing +
                                  BEARING_FUDGE)
        else:
            return False

        return math.fabs(self.brain.ball.bearing - shootingObjBearing) < 20.
