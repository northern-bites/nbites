import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
from ..navigator import Navigator
from ..util import *
import noggin_constants as NogginConstants
from objects import Location, RelRobotLocation
from . import RoleConstants as roleConstants
from . import SharedTransitions
import math

@superState('gameControllerResponder')
def positionReady(player):
    """
    Game Ready positioning
    """
    if player.firstFrame():
        if player.brain.gameController.ownKickOff and player.isKickingOff:
            player.kickoffPosition = roleConstants.ourKickoff
        elif player.isKickingOff:
            player.kickoffPosition = roleConstants.theirKickoff

        player.brain.nav.goTo(player.kickoffPosition,
                              precision = Navigator.PLAYBOOK,
                              speed = Navigator.QUICK_SPEED,
                              avoidObstacles = True,
                              fast = True, pb = False)

        player.brain.tracker.repeatBasicPan()

    if player.brain.nav.isAtPosition():
        player.brain.tracker.trackBall()
        return player.stay()

    if player.brain.time - player.timeReadyBegan > 36.0:
        return player.goNow('readyFaceMiddle')

    return player.stay()

@superState('gameControllerResponder')
def readyFaceMiddle(player):
    """
    If we didn't make it to our position, find the middle of the field
    """
    yglp = player.brain.yglp
    ygrp = player.brain.ygrp
    rbearing = math.degrees(ygrp.bearing)
    lbearing = math.degrees(yglp.bearing)

    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)
        player.stand()
        readyFaceMiddle.startedSpinning = False
        readyFaceMiddle.done = False
        readyFaceMiddle.spin = False

    centerField = Location(NogginConstants.CENTER_FIELD_X,
                           NogginConstants.CENTER_FIELD_Y)

    if player.brain.nav.isStopped() and not readyFaceMiddle.startedSpinning:
        readyFaceMiddle.startedSpinning = True
        spinDir = player.brain.loc.spinDirToPoint(centerField)
        player.setWalk(0,0,spinDir*.5)

    elif (not readyFaceMiddle.done and readyFaceMiddle.startedSpinning and
        ((ygrp.distance > NogginConstants.MIDFIELD_X + 100) or
        (yglp.distance > NogginConstants.MIDFIELD_X + 100)) and
        ((math.fabs(rbearing) < 10.0 and rbearing != 0.0) or
         (math.fabs(lbearing) < 10.0 and lbearing != 0.0))):

        readyFaceMiddle.done = True
        player.brain.tracker.repeatBasicPan()
        player.stopWalking()

    return player.stay()

readyFaceMiddle.done = False
readyFaceMiddle.startedSpinning = False
