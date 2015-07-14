"""
@Summer 2013: This entire state is an aid for localization, since we can only
hard reset to one of the two possible post-penalty positions.
"""

import ChaseBallTransitions as transitions
import RoleConstants as roleConstants
from noggin_constants import LineID
from math import copysign, fabs, pi
from objects import RelRobotLocation
from ..navigator import Navigator
from ..util import *

DEBUG_PENALTY_STATES = False

@superState('gameControllerResponder')
def afterPenalty(player):

    if player.firstFrame():
        if DEBUG_PENALTY_STATES:
            print "Entering the 'afterPenalty' state."

        afterPenalty.right = True
        afterPenalty.decidedSide = False
        player.brain.tracker.lookToAngle(-90)

        # reset state specific counters
        afterPenalty.cornerOn = 0
        afterPenalty.cornerFrames = 0
        afterPenalty.stateCount = 0

    afterPenalty.stateCount += 1
    vis = player.brain.vision

    # Alternate between looking right and left
    if afterPenalty.stateCount % 70 == 0:
        afterPenalty.right = not afterPenalty.right
        if afterPenalty.right:
            player.brain.tracker.lookToAngle(-90)
        else:
            player.brain.tracker.lookToAngle(90)
        # Reset counters
        afterPenalty.cornerOn = 0
        afterPenalty.cornerFrames = 0

    foundCorner = False
    for i in range(0, vis.corner_size()):
        corner = vis.corner(i)
        # We do NOT want to use T corners
        if corner.id != 2:
            foundCorner = True

    # Only count if we're looking right/left. Don't want to count goalbox corners
    if player.brain.tracker.isStopped():
        if foundCorner:
            afterPenalty.cornerOn += 1
        afterPenalty.cornerFrames += 1

    # Only decide we're good to go if we saw corner in > 2/3 of frames
    if afterPenalty.cornerFrames > 30:
        ratio = afterPenalty.cornerOn / float(afterPenalty.cornerFrames)
        if ratio > .66:
            afterPenalty.decidedSide = True

    if afterPenalty.decidedSide:
        player.brain.resetLocalizationFromPenalty(afterPenalty.right)
        if DEBUG_PENALTY_STATES:
            print "We've decided that the goal's to our right? " + str(afterPenalty.right)

        player.brain.tracker.repeatWidePan()

        if not roleConstants.isGoalie(player.role):
            return player.goNow('walkOut')
        return player.goLater(player.gameState)

    return player.stay()

@superState('gameControllerResponder')
def walkOut(player):
    player.brain.nav.destinationWalkTo(RelRobotLocation(100, 0, 0),
                                       Navigator.BRISK_SPEED)
    
    if player.stateTime > 5:
        return player.goNow('determineRole')
    return player.stay()

@superState('gameControllerResponder')
def determineRole(player):
    if not player.roleSwitching:
        return player.goLater(player.gameState)

    openSpaces = [True, True, True, True]
    for mate in player.brain.teamMembers:
        if mate.playerNumber == player.brain.playerNumber:
            continue
        if not roleConstants.isGoalie(mate.role) \
                and mate.frameSinceActive < 30:
            openSpaces[mate.role - 2] = False

    position = 0

    for i in range(4):
        if openSpaces[i] and roleConstants.canRoleSwitchTo(i+2):
            roleConstants.setRoleConstants(player, i+2)
            return player.goLater(player.gameState)
        elif openSpaces[i]:
            position = i+2

    if position == 0:
        print "Came out of penalty and found no open spaces!!!"

    roleConstants.setRoleConstants(player, i+2)
    return player.goLater(player.gameState)

