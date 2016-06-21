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

# So this entire state is cobbled together in the rush of China 2015.
# *IDEALLY* we would use exclusively corners and actively track them,
# I don't have time to do this so we're just going a static 80 degrees.

# Someone could really knock this out of the park with a good day of work,
# alternatively this could be a good newbie project. Please fix though

@superState('gameControllerResponder')
def afterPenalty(player):

    if player.firstFrame():
        if DEBUG_PENALTY_STATES:
            print "Entering the 'afterPenalty' state."

        # Hackity Hackity Hack. Determine if we've been manually positioned or are actually coming out of penalty
        if player.brain.penalizedHack:
            closeRatio = player.brain.penalizedEdgeClose / float(player.brain.penalizedCount)
            print "Close ratio is: ", closeRatio
            if closeRatio < .5:
                player.brain.resetLocToCross()
                return player.goLater(player.gameState)

        afterPenalty.right = True
        afterPenalty.decidedSide = False
        player.brain.tracker.lookToAngle(-1 * angle)

        # state specific counters
        afterPenalty.rightDiff = 0
        afterPenalty.leftDiff = 0
        afterPenalty.cornerCOn = 0
        afterPenalty.cornerTOn = 0

        afterPenalty.leftHorizSum = 0
        afterPenalty.rightHorizSum = 0
        afterPenalty.stateCount = 0

    afterPenalty.stateCount += 1
    vis = player.brain.vision

    # Alternate between looking right and left
    if afterPenalty.stateCount % 70 == 0:
        if afterPenalty.right:
            afterPenalty.rightDiff += afterPenalty.cornerCOn - afterPenalty.cornerTOn
            if DEBUG_PENALTY_STATES:
                print "After Penalty right Diff: ", afterPenalty.rightDiff, " cOn: ", afterPenalty.cornerCOn, " tOn: ", afterPenalty.cornerTOn
                print "Right sum: ", afterPenalty.rightHorizSum
                #print "     right close: ", player.brain
            player.brain.tracker.lookToAngle(angle)
        else:
            player.brain.tracker.lookToAngle(-1* angle)
            afterPenalty.leftDiff += afterPenalty.cornerCOn - afterPenalty.cornerTOn
            if DEBUG_PENALTY_STATES:
                print "After Penalty left Diff: ", afterPenalty.leftDiff, " cOn: ", afterPenalty.cornerCOn, " tOn: ", afterPenalty.cornerTOn
                print "Left sum: ", afterPenalty.leftHorizSum
        afterPenalty.right = not afterPenalty.right
        # Reset counters
        afterPenalty.cornerCOn = 0
        afterPenalty.cornerTOn = 0

    foundCorner = False

    # Only count if we're looking right/left. Don't want to count anything while panning
    if player.brain.tracker.isStopped():
        if afterPenalty.right:
            afterPenalty.rightHorizSum += player.brain.vision.horizon_dist
        else:
            afterPenalty.leftHorizSum += player.brain.vision.horizon_dist

        for i in range(0, vis.corner_size()):
            corner = vis.corner(i)
            if corner.id == 2:
                afterPenalty.cornerTOn += 1
            else:
                afterPenalty.cornerCOn += 1

    # China hack 2015. Please make this better. Fairly arbitrary thresholds.
    # The idea is if we're flip-flopping between T and C classifications we
    # have bigger vision problems going on.
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
        if afterPenalty.decidedSide:
            player.brain.resetLocalizationFromPenalty(afterPenalty.right)
        else:
            # This is a backup. We would prefer to use corners to determine our side as fieldedge could be noisy
            print "Corners didn't work! defaulting to horizonDist"
            player.brain.resetLocalizationFromPenalty(afterPenalty.rightHorizSum < afterPenalty.leftHorizSum)
        if DEBUG_PENALTY_STATES:
            print "We've decided! ", afterPenalty.right, " LeftDiff: ", afterPenalty.leftDiff, \
                " Right Diff: ", afterPenalty.rightDiff, " StateCount: ", afterPenalty.stateCount
            print "RSUM: ", afterPenalty.rightHorizSum, " LSUM: ", afterPenalty.leftHorizSum

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

