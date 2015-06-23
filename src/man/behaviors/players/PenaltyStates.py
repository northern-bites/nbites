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
OBJ_SEEN_THRESH = 6

# TODO we can also check for side of goalbox or other line ids
@superState('gameControllerResponder')
def afterPenalty(player):

    if player.firstFrame():

        if DEBUG_PENALTY_STATES:
            print "Entering the 'afterPenalty' state."

        # pan for the ball
        player.brain.tracker.repeatWidePan()
        
        # reset state specific counters
        afterPenalty.goalLeft = 0
        afterPenalty.reset_loc = 0

    lines = player.brain.visionLines

    # Do we see the top of the goalbox
    for i in range(0, lines.line_size()):
        if lines.line(i).id == LineID.TopGoalbox:
            topGoalBox = lines.line(i).inner
            leftAngle = fabs(topGoalBox.t - pi) < .25
            # Goalbox to the left = 1 to the right = -1
            toLeft = 1 if leftAngle else -1
            afterPenalty.goalLeft += toLeft

    # If we've seen any landmark enough, reset localization.
    if fabs(afterPenalty.goalLeft) > OBJ_SEEN_THRESH:
        if DEBUG_PENALTY_STATES:
            print "Saw goalbox enough times"
        afterPenalty.reset_loc = copysign(1, afterPenalty.goalLeft)

    # Send the reset loc command.
    if afterPenalty.reset_loc != 0:
        if DEBUG_PENALTY_STATES:
            print "Sufficient sightings. reset_loc value is: " + str(afterPenalty.reset_loc)
        afterPenalty.goalLeft += afterPenalty.reset_loc
        afterPenalty.reset_loc = 0

    if fabs(afterPenalty.goalLeft) > 5 or player.stateTime > 15:
        if DEBUG_PENALTY_STATES:
            print "Consensus reached! Resetting loc. Is the goal to our right? " + str(afterPenalty.goalLeft < 0)
        # Yes, when goal_right is less than 0, our goal is to our right.
        # It seems counter intuitive, but that's how it works. -Josh Z
        if afterPenalty.goalLeft > 0:
            print "Goal is to left. Coming out of penalty"
        else:
            print "Goal is to right. Coming out of penalty"
        player.brain.resetLocalizationFromPenalty(afterPenalty.goalLeft < 0)
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
            # US Open hack
            if player.brain.game:
                oppTeam = player.brain.game.team(1).team_number
            else:
                oppTeam = -1
            roleConstants.setRoleConstants(player, i+2, oppTeam)
            return player.goLater(player.gameState)
        elif openSpaces[i]:
            position = i+2

    if position == 0:
        print "Came out of penalty and found no open spaces!!!"
    # US Open hack
    if player.brain.game:
        oppTeam = player.brain.game.team(1).team_number
    else:
        oppTeam = -1
    roleConstants.setRoleConstants(player, i+2, oppTeam)
    return player.goLater(player.gameState)

