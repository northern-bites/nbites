"""
Game controller states for pUNSW, our soccer player.
"""

from math import fabs
from ..util import *
from .. import SweetMoves
import KickOffConstants as kickOff

import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
from ..navigator import Navigator
from ..navigator import BrunswickSpeeds as speeds
import noggin_constants as NogginConstants
from objects import Location, RelRobotLocation
from . import RoleConstants as roleConstants
from . import SharedTransitions


### Change these for picture taking ###
FRAME_SAVE_RATE = 1
NUM_FRAMES_TO_SAVE = 150

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        print("I'm in UNSW initial frame!")
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.gainsOn()
        player.stand()
        player.zeroHeads()
        player.brain.resetInitialLocalization()
        player.lastStiffStatus = True
        #Reset role to player number
        player.role = player.brain.playerNumber
        roleConstants.setRoleConstants(player, player.role)

    # If stiffnesses were JUST turned on, then stand up.
    if player.lastStiffStatus == False and player.brain.interface.stiffStatus.on:
        player.stand()
    # Remember last stiffness.
    player.lastStiffStatus = player.brain.interface.stiffStatus.on

    return player.stay()


@superState('gameControllerResponder')
def gameReady(player):
    if player.firstFrame():
        print("I'm in UNSW first frame!")
        if player.brain.gameController.ownKickOff and player.isKickingOff:
            player.kickoffPosition = roleConstants.ourKickoff
        elif player.isKickingOff:
            player.kickoffPosition = roleConstants.theirKickoff

        myLocation = RelRobotLocation(40, 0, 0)

        player.brain.nav.goTo(myLocation,
                              precision = Navigator.PLAYBOOK,
                              speed = speeds.SPEED_SEVEN,
                              avoidObstacles = True,
                              fast = True, pb = False)

        player.brain.tracker.repeatBasicPan()

    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    if player.firstFrame():
        player.stand()
        player.brain.tracker.lookToAngle(0)
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        player.panIndex = 0
        return player.goNow('panTop')
    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    if player.firstFrame():
        player.panIndex = 0
        return player.goNow('panBottom')
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    player.inKickingState = False
    return player.stay()