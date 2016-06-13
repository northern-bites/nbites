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

DEBUG_PENALTY_STATES = True
CHECK_VALS_EACH_PAN = True
SCRIMMAGE = True
angle = 80

# So this entire state is cobbled together in the rush of China 2015.
# *IDEALLY* we would use exclusively corners and actively track them,
# I don't have time to do this so we're just going a static 80 degrees.

# Someone could really knock this out of the park with a good day of work,
# alternatively this could be a good newbie project. Please fix though

@superState('gameControllerResponder')
def afterPenalty(player):

    ## TODO TEST VALUES: NUMBER OF FRAMES AND RIGHT/LEFT DIFFS
    # TODO add a check for whistle override

    if player.firstFrame():
        if DEBUG_PENALTY_STATES:
            print "Entering the 'afterPenalty' state; DEBUG_PENALTY_STATES IS ON."


        afterPenalty.decidedSide = False
        afterPenalty.lookRight = True

        # count the number of times we see the goalbox and L-Corners on the right
        afterPenalty.innerLCornerRight = 0
        afterPenalty.outerLCornerRight = 0
        afterPenalty.goalboxRight = 0

        # count the number of times we see the CC and T-Corner on the left
        afterPenalty.tCornerLeft = 0
        afterPenalty.CenterCircleLeft = 0

        ## VARS FROM 2015 AFTERPENALTY ##
        afterPenalty.right = True
        afterPenalty.decidedSide = False
        player.brain.tracker.lookToAngle(-1 * angle)

        # number of times we see the horizon on either side - if corner check fails, try using this.
        afterPenalty.leftHorizSum = 0
        afterPenalty.rightHorizSum = 0

        # Keep track of frames since player was penalized
        afterPenalty.frameCount = 0

        # For debugging: keep track of the number of head pans.
        afterPenalty.numOfPans = 0

    # Update number of frames since player entered
    afterPenalty.frameCount += 1
    vis = player.brain.vision

    if afterPenalty.frameCount % 50 == 0:

        # LOOK RIGHT
        if afterPenalty.lookRight:
            player.brain.tracker.lookToAngle(angle)
            if DEBUG_PENALTY_STATES and CHECK_VALS_EACH_PAN:
                print "Looking to my right! Angle:", angle
                print "I've seen an inside L-Corner", afterPenalty.innerLCornerRight, "outer L-Corner", afterPenalty.outerLCornerRight, "times, T-Corner", afterPenalty.tCornerLeft, "times"
                print "I've seen the goalbox", afterPenalty.goalboxRight, "times, Center Circle", afterPenalty.CenterCircleLeft, "times"
                print "Cumulative horizon distance on this side is", afterPenalty.leftHorizSum

        # LOOK LEFT
        else:
            otherAngle = -1 * angle
            player.brain.tracker.lookToAngle(-1 * angle)
            if DEBUG_PENALTY_STATES and CHECK_VALS_EACH_PAN:
                print "Looking to my left! Angle:", otherAngle
                print "I've seen an inside L-Corner", afterPenalty.innerLCornerRight, "outer L-Corner", afterPenalty.outerLCornerRight, "times, T-Corner", afterPenalty.tCornerLeft, "times"
                print "I've seen the goalbox", afterPenalty.goalboxRight, "times, Center Circle", afterPenalty.CenterCircleLeft, "times"
                print "Cumulative horizon distance on this side is", afterPenalty.rightHorizSum

        afterPenalty.lookRight = not afterPenalty.lookRight

        if DEBUG_PENALTY_STATES:
            afterPenalty.numOfPans += 1

    # Look for landmarks after we've panned.
    # Corner IDs:
    ## 0: Inner L-Corner
    ## 1: Outer L-Corner
    ## 2: T-Corner
    ## 3: Center Circle Corner

    if player.brain.tracker.isStopped():
        if afterPenalty.lookRight:

            # TODO For now, accumulate the horizon distance on this side; eventually average?
            afterPenalty.rightHorizSum += vis.horizon_dist

            # Update the corner counters according to what we see on this side.
            for i in range(0, vis.corner_size()):  
                corner = vis.corner(i)
                if corner.id == 0:
                    afterPenalty.innerLCornerRight += 1
                if corner.id == 1:
                    afterPenalty.outerLCornerRight += 1
                if corner.id == 2:
                    afterPenalty.tCornerLeft -= 1
                if corner.id == 3: 
                    afterPenalty.CenterCircleLeft -= 1

        else:
            afterPenalty.leftHorizSum += vis.horizon_dist
            for i in range(0, vis.corner_size()):
                corner = vis.corner(i)
                if corner.id == 0:
                    afterPenalty.innerLCornerRight -= 1
                if corner.id == 1:
                    afterPenalty.outerLCornerRight -=1
                if corner.id == 2:
                    afterPenalty.tCornerLeft += 1
                if corner.id == 3:
                    afterPenalty.CenerCircleLeft += 1

    # TODO define thresholds and hierarchy. If all numbers are negaive, the goalbox is on our left.
    # TODO need to switch 'decidedSide'... should we keep panning until we've decided, or proceed when we're unsure?
    if afterPenalty.frameCount > 200:

        # arbitrary thresholds for now!
        if afterPenalty.tCornerLeft > 20 or afterPenalty.outerLCornerRight > 20:
            if DEBUG_PENALTY_STATES:
                print "Player thinks the goal is on the right!"
            afterPenalty.decidedSide = True

        elif afterPenalty.tCornerLeft < -20 or afterPenalty.outerLCornerRight < -20:
            if DEBUG_PENALTY_STATES:
                print "Player thinks the goal is on the left!"
            afterPenalty.decidedSide = True
            afterPenalty.right = False

    if afterPenalty.frameCount > 300 or afterPenalty.decidedSide:
        if afterPenalty.decidedSide:
            player.brain.resetLocalizationFromPenalty(afterPenalty.right)

        else:
            if DEBUG_PENALTY_STATES:
                print("THRESHOLDS WERE NOT MET! Rely on Horizons!")
                # TODO test if horizons are actually more reliable.
                # See if this helps if we're blocking corners
            player.brain.resetLocalizationFromPenalty(afterPenalty.leftHorizSum > afterPenalty.rightHorizSum)

        if DEBUG_PENALTY_STATES:
            print ("\n-------------------------------------------------------------")
            print("COUNTER TOTALS: ")
            print ("innerLCornerRight:", afterPenalty.innerLCornerRight, "outerLCornerRight:", afterPenalty.outerLCornerRight, "goalboxRight:", \
                afterPenalty.goalboxRight, "tCornerLeft:", afterPenalty.tCornerLeft, "CenterCircleLeft:", afterPenalty.CenterCircleLeft)
            print("HORIZON DISTANCE TOTALS:")
            print ("left horizon:", afterPenalty.leftHorizSum, "right horizon", afterPenalty.rightHorizSum)
            print ("-------------------------------------------------------------\n")
            if not SCRIMMAGE:
                return player.goNow('gamePenalized')

        # TODO see if the goalie role affects this
        player.brain.tracker.lookToAngle(0)
        return player.goNow('walkOut')

    # Stay until we've finished checking out our surroundings
    return player.stay()


@superState('gameControllerResponder')
def manualPlacement(player):
    """
    TODO / OPTIONAL for now: state in which a robot can still localize after being manually placed.
    We shouldn't go into after penalty in this case, but the GameController can still penalize us in
    GameReady, so this could be useful.

    """

    if player.firstFrame() and (player.lastDiffState == 'GameReady' or player.lastDiffState == 'GameSet'):
        print "Going into after penalty from ready or set; am I being manually placed?"
        player.wasPenalized = False
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

