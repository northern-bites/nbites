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
angle = 80

@superState('gameControllerResponder')
def afterPenalty(player):

    if player.firstFrame():
        if DEBUG_PENALTY_STATES:
            print "Entering the 'afterPenalty' state."

        afterPenalty.right = True
        afterPenalty.decidedSide = False
        player.brain.tracker.lookToAngle(-1 * angle)

        # state specific counters
        afterPenalty.rightDiff = 0
        afterPenalty.leftDiff = 0
        afterPenalty.cornerCOn = 0
        afterPenalty.cornerTOn = 0
        afterPenalty.cornerFrames = 0
        afterPenalty.stateCount = 0

    afterPenalty.stateCount += 1
    vis = player.brain.vision

    # Alternate between looking right and left
    if afterPenalty.stateCount % 70 == 0:
        if afterPenalty.right:
            afterPenalty.rightDiff += afterPenalty.cornerCOn - afterPenalty.cornerTOn
            if DEBUG_PENALTY_STATES:
                print "After Penalty right Diff: ", afterPenalty.rightDiff, " cOn: ", afterPenalty.cornerCOn, " tOn: ", afterPenalty.cornerTOn
            player.brain.tracker.lookToAngle(angle)
        else:
            player.brain.tracker.lookToAngle(-1* angle)
            afterPenalty.leftDiff += afterPenalty.cornerCOn - afterPenalty.cornerTOn
            if DEBUG_PENALTY_STATES:
                print "After Penalty left Diff: ", afterPenalty.leftDiff, " cOn: ", afterPenalty.cornerCOn, " tOn: ", afterPenalty.cornerTOn
        afterPenalty.right = not afterPenalty.right
        # Reset counters
        afterPenalty.cornerCOn = 0
        afterPenalty.cornerTOn = 0
        # afterPenalty.cornerFrames = 0

    foundCorner = False

    # Only count if we're looking right/left. Don't want to count goalbox corners
    if player.brain.tracker.isStopped():
        afterPenalty.cornerFrames += 1
        for i in range(0, vis.corner_size()):
            corner = vis.corner(i)
            if corner.id == 2:
                afterPenalty.cornerTOn += 1
            else:
                afterPenalty.cornerCOn += 1

    # China hack 2015. Please make this better
    if afterPenalty.stateCount > 140:
        if afterPenalty.rightDiff > 5 and afterPenalty.leftDiff <= 0:
            afterPenalty.decidedSide = True
            afterPenalty.right = True
        elif afterPenalty.rightDiff < -5 and afterPenalty.leftDiff >= 0:
            afterPenalty.decidedSide = True
            afterPenalty.right = False
        elif afterPenalty.leftDiff > 5 and afterPenalty.rightDiff <= 0:
            afterPenalty.decidedSide = True
            afterPenalty.right = False
        elif afterPenalty.leftDiff < -5 and afterPenalty.rightDiff >= 0:
            afterPenalty.decidedSide = True
            afterPenalty.right = True

    if afterPenalty.decidedSide or afterPenalty.stateCount > 300:
        player.brain.resetLocalizationFromPenalty(afterPenalty.right)
        if DEBUG_PENALTY_STATES:
            print "We've decided! ", afterPenalty.right, " LeftDiff: ", afterPenalty.leftDiff, " Right Diff: ", afterPenalty.rightDiff, " StateCount: ", afterPenalty.stateCount

        player.brain.tracker.repeatWidePan()

        if not roleConstants.isGoalie(player.role):
            return player.goNow('walkOut')
        return player.goLater(player.gameState)

    return player.stay()

@superState('gameControllerResponder')
def walkOut(player):
    player.brain.nav.walk(0.5, 0, 0)

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

