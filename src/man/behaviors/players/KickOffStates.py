from ..util import *
from . import RoleConstants as roleConstants
import noggin_constants as nogginC
from objects import Location
from ..navigator import Navigator
from ..kickDecider import KickDecider
import KickOffConstants as constants

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
            corner = Location(nogginC.FIELD_WHITE_WIDTH, 0.)
            decider = KickDecider.KickDecider(player.brain)
            player.kick = decider.sweetMovesForKickOff(0, corner)
            player.inKickingState = True
            return player.goNow('approachBall')
        elif roleConstants.isSecondChaser(player.role) or roleConstants.isCherryPicker(player.role):
            player.brain.tracker.repeatFastNarrowPan()
            passDestination = Location(nogginC.OPP_GOALBOX_LEFT_X, 60.)
            player.brain.nav.goTo(passDestination, Navigator.GENERAL_AREA, Navigator.QUICK_SPEED, 
                                True, False, True, False)
        else:
            return player.goNow('playOffBall')

        return player.stay()

    elif constants.shouldPassToFieldCross(player):
        return player.goNow('passToFieldCross')
    elif constants.ballNotPassedToCorner(player):
        player.passBack = False
        player.inKickOffPlay = False
        return player.goNow('findBall')
    else:
        passDestination = Location(nogginC.OPP_GOALBOX_LEFT_X, 60.)
        player.brain.nav.updateDest(passDestination)

    return player.stay()

@superState('gameControllerResponder')
def passToFieldCross(player):
    if player.firstFrame():
        player.passBack = False
        if roleConstants.isFirstChaser(player.role):
            player.brain.tracker.repeatFastNarrowPan()
            fieldCross = Location(nogginC.LANDMARK_OPP_FIELD_CROSS[0], nogginC.LANDMARK_OPP_FIELD_CROSS[1])
            player.brain.nav.goTo(fieldCross, Navigator.GENERAL_AREA, Navigator.QUICK_SPEED, 
                                True, False, True, False)

        elif roleConstants.isSecondChaser(player.role) or roleConstants.isCherryPicker(player.role):
            if not constants.ballIsLost(player):
                decider = KickDecider.KickDecider(player.brain)
                # player.kick = decider.bigKicksOnGoal()
                # player.kick = decider.sweetMovesOnGoal()
                # player.kick = decider.sweetMoveCrossToCenter()
                player.kick = decider.attacker()
                player.finishedPlay = True
                player.inKickingState = True
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
            passDest = Location(nogginC.MIDFIELD_X + constants.SIDE_PASS_OFFSET, 0.)
            player.kick = decider.sweetMovesForKickOff(-1, passDest) 
            player.inKickingState = True
            return player.goNow('approachBall')
        elif roleConstants.isSecondChaser(player.role) or roleConstants.isCherryPicker(player.role):
            pass
        else:
            return player.goNow('playOffBall')

        return player.stay()

    if constants.sidePassFinished(player):
        if constants.isSeeingBall(player):
            return player.goNow('passToFieldCross')
        elif constants.didNotRecieveSidePass(player):
            player.passBack = False
            player.inKickOffPlay = False
            return player.goNow('findBall')

    return player.stay()
