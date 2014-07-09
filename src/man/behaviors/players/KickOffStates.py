from ..util import *
from . import RoleConstants as roleConstants
import noggin_constants as nogginC
from objects import Location
from ..navigator import Navigator
from ..kickDecider import KickDecider
import KickOffConstants as constants

#TODO add comm field to pass if one is aborting the play
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
        else:
            return player.goNow('playOffBall')

    if constants.shouldPassToFieldCross(player):
        return player.goNow('passToFieldCross')
    elif constants.ballNotPassedToCorner(player):
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

        elif roleConstants.isSecondChaser(player.role):
            if not constants.ballIsLost(player):
                decider = KickDecider.KickDecider(player.brain)
                # player.kick = decider.bigKicksOnGoal()
                player.kick = decider.sweetMovesOnGoal()
                player.inKickingState = True
                # player.kick = decider.sweetMoveCrossToCenter()
                player.finishedPlay = True
                return player.goNow('approachBall')
            else:
                player.inKickOffPlay = False
                return player.goNow('findBall')
        else:
            return player.goNow('playOffBall')

        return player.stay()

    if roleConstants.isFirstChaser(player.role):
        fieldCross = Location(nogginC.LANDMARK_OPP_FIELD_CROSS[0], nogginC.LANDMARK_OPP_FIELD_CROSS[1])
        player.brain.nav.updateDest(fieldCross)
        if constants.shouldStopWalkingToCross(player):
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
        player.passBack = True
        if roleConstants.isFirstChaser(player.role):
            decider = KickDecider.KickDecider(player.brain)
            # TODO this is the correct side for games
            # passDest = Location(nogginC.MIDFIELD_X + 20., 0.)
            # player.kick = decider.sweetMovesForKickOff(-1, passDest)
            passDest = Location(nogginC.MIDFIELD_X, nogginC.FIELD_WHITE_HEIGHT)
            player.kick = decider.sweetMovesForKickOff(1, passDest)   
            player.inKickingState = True
            return player.goNow('approachBall')
        elif roleConstants.isSecondChaser(player.role):
            pass
        else:
            return player.goNow('playOffBall')

        return player.stay()

    if constants.sidePassFinished(player)
        if constants.isSeeingBall(player):
            return player.goNow('passToFieldCross')
        else:
            player.passBack = False
            player.inKickOffPlay = False
            return player.goNow('findBall')

    return player.stay()

#giveAndGo -> give to corner, go to field cross
#giveAndGo2 -> sideKick to shot on goal, to run to field cross
#shoot from corner