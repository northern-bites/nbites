from ..util import *
from . import RoleConstants as roleConstants
import noggin_constants as nogginC
from objects import RelRobotLocation, Location
from ..navigator import Navigator
import ChaseBallTransitions as transitions
from ..kickDecider import KickDecider

@superState('gameControllerResponder')
def giveAndGo(player):
    player.finishedPlay = False
    player.shouldKickOff = False
    return player.goNow('passToCorner')

@superState('gameControllerResponder')
def passToCorner(player):
    if player.firstFrame():
        player.passBack = True
        if roleConstants.isFirstChaser(player.role):
            decider = KickDecider.KickDecider(player.brain)
            player.kick = decider.motionKicksForKickOff()
            player.inKickingState = True
            return player.goNow('approachBall')
        elif roleConstants.isSecondChaser(player.role):
            player.brain.tracker.lookStraight()
            player.setWalk(.7, 0., 0.)

    if player.stateTime > 6 and transitions.shouldChaseBall(player):
        return player.goNow('passToFieldCross')
    elif player.stateTime > 10:
        return player.goNow('findBall')

    return player.stay()

@superState('gameControllerResponder')
def passToFieldCross(player):
    if player.firstFrame():
        player.give = False
        passToFieldCross.count = 0

        if roleConstants.isFirstChaser(player.role):
            player.brain.tracker.lookStraight()
            #fieldCrossX = nogginC.LANDMARK_OPP_FIELD_CROSS[0] - player.brain.loc.x
            #fieldCrossY = nogginC.LANDMARK_OPP_FIELD_CROSS[1] - player.brain.loc.y
            #fieldCrossH = -1*player.brain.loc.h
            #fieldCross = RelRobotLocation(fieldCrossX, fieldCrossY, fieldCrossH)
            #player.brain.nav.destinationWalkTo(fieldCross,
            #                        Navigator.QUICK_SPEED)
            fieldCross = Location(nogginC.LANDMARK_OPP_FIELD_CROSS[0], nogginC.LANDMARK_OPP_FIELD_CROSS[1])
            player.brain.nav.goTo(fieldCross, Navigator.GENERAL_AREA, Navigator.QUICK_SPEED, 
                                True, False, True, False)

        elif roleConstants.isSecondChaser(player.role) and transitions.shouldChaseBall(player):
            decider = KickDecider.KickDecider(player.brain)
            player.kick = decider.motionKicksOnGoal()
            player.finishedPlay = True
            return player.goNow('approachBall')

        return player.stay()

    passToFieldCross.count = passToFieldCross.count + 1
    if roleConstants.isFirstChaser(player.role):
        # fieldCrossX = nogginC.LANDMARK_OPP_FIELD_CROSS[0] - player.brain.loc.x
        # fieldCrossY = nogginC.LANDMARK_OPP_FIELD_CROSS[1] - player.brain.loc.y
        # fieldCrossH = -1*player.brain.loc.h
        # fieldCross = RelRobotLocation(fieldCrossX, fieldCrossY, fieldCrossH)
        # player.brain.nav.updateDestinationWalkDest(fieldCross)
        fieldCross = Location(nogginC.LANDMARK_OPP_FIELD_CROSS[0], nogginC.LANDMARK_OPP_FIELD_CROSS[1])
        player.brain.nav.updateDest(fieldCross)
        if passToFieldCross.count > 300 and transitions.shouldChaseBall(player):
            player.inKickOffPlay = False
            return player.goNow('approachBall')

    return player.stay()

@superState('gameControllerResponder')
def giveAndGo2(player):
    player.finishedPlay = False
    player.shouldKickOff = False
    return player.goNow('sidePass')

#giveAndGo -> give to corner, go to field cross
#giveAndGo2 -> sideKick to shot on goal, to run to field cross
#shoot from corner