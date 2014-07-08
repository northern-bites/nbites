from ..util import *
from . import RoleConstants as roleConstants
import noggin_constants as nogginC
from objects import RelRobotLocation, Location
from ..navigator import Navigator
import ChaseBallTransitions as transitions
from ..kickDecider import KickDecider
from ..kickDecider import kicks

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
            # TODO wrong corner
            corner = Location(nogginC.FIELD_WHITE_WIDTH, nogginC.FIELD_WHITE_HEIGHT)
            decider = KickDecider.KickDecider(player.brain)
            player.kick = decider.sweetMovesForKickOff(0, corner)
            player.inKickingState = True
            return player.goNow('approachBall')
        elif roleConstants.isSecondChaser(player.role):
            player.brain.tracker.lookStraight()
            player.setWalk(.7, 0., 0.)

    if player.stateTime > 9 and transitions.shouldChaseBall(player):
        return player.goNow('passToFieldCross')
    elif player.stateTime > 14:
        player.passBack = False
        player.inKickOffPlay = False
        return player.goNow('findBall')

    return player.stay()

@superState('gameControllerResponder')
def passToFieldCross(player):
    if player.firstFrame():
        player.passBack = False
        if roleConstants.isFirstChaser(player.role):
            player.brain.tracker.lookStraight()
            fieldCross = Location(nogginC.LANDMARK_OPP_FIELD_CROSS[0], nogginC.LANDMARK_OPP_FIELD_CROSS[1])
            player.brain.nav.goTo(fieldCross, Navigator.GENERAL_AREA, Navigator.QUICK_SPEED, 
                                True, False, True, False)

        elif roleConstants.isSecondChaser(player.role) and transitions.shouldChaseBall(player):
            decider = KickDecider.KickDecider(player.brain)
            # player.kick = decider.bigKicksOnGoal()
            # player.kick = decider.sweetMovesOnGoal()
            player.inKickingState = True
            player.kick = decider.sweetMoveCrossToCenter()
            player.finishedPlay = True
            return player.goNow('approachBall')

        return player.stay()

    if roleConstants.isFirstChaser(player.role):
        fieldCross = Location(nogginC.LANDMARK_OPP_FIELD_CROSS[0], nogginC.LANDMARK_OPP_FIELD_CROSS[1])
        player.brain.nav.updateDest(fieldCross)
        if player.stateTime > 12 and transitions.shouldChaseBall(player):
            player.inKickOffPlay = False
            return player.goNow('approachBall')

    return player.stay()

@superState('gameControllerResponder')
def giveAndGo2(player):
    player.finishedPlay = False
    player.shouldKickOff = False
    return player.goNow('sidePass')

@superState('gameControllerResponder')
def sidePass(player):
    if player.firstFrame():
        if roleConstants.isFirstChaser(player.role):
            decider = KickDecider.KickDecider(player.brain)
            # TODO this is the correct side for games
            # passDest = Location(nogginC.MIDFIELD_X + 20., 0.)
            # player.kick = decider.sweetMovesForKickOff(-1, passDest)
            passDest = Location(nogginC.MIDFIELD_X, nogginC.FIELD_WHITE_HEIGHT)
            player.kick = decider.sweetMovesForKickOff(1, passDest)
            player.passBack = True
            player.inKickingState = True
            return player.goNow('approachBall')

    if roleConstants.isSecondChaser(player.role) and player.brain.gameController.timeSincePlaying > 8:
        if transitions.shouldChaseBall(player):
            return player.goNow('passToFieldCross')
        else:
            player.passBack = False
            player.inKickOffPlay = False
            return player.goNow('findBall')

    return player.stay()

#giveAndGo -> give to corner, go to field cross
#giveAndGo2 -> sideKick to shot on goal, to run to field cross
#shoot from corner