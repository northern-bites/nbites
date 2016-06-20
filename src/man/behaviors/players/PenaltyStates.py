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
DEBUG_MANUAL_PLACEMENT = False
CHECK_VALS_EACH_PAN = False
DEBUG_HORIZONS = False
SCRIMMAGE = True
angle = 80
manualPlacement_angle = 65

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

        
        return player.goNow('overeagerWhistle')

        afterPenalty.decidedSide = False
        afterPenalty.lookRight = True

        # count the number of times we see the goalbox and L-Corners on the right
        afterPenalty.innerLCornerRight = 0
        afterPenalty.outerLCornerRight = 0

        # count the number of times we see the CC and T-Corner on the left
        afterPenalty.tCornerLeft = 0

        # Looking to the right, whether we've determined what side we're on
        afterPenalty.right = True
        afterPenalty.decidedSide = False
        player.brain.tracker.lookToAngle(-1 * angle) # pan immediately
        afterPenalty.leftHorizSum = 0
        afterPenalty.rightHorizSum = 0
        afterPenalty.averageLeftHorizon = 0
        afterPenalty.averageRightHorizon = 0

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
                print "Cumulative horizon distance on this side is", afterPenalty.leftHorizSum

        # LOOK LEFT
        else:
            otherAngle = -1 * angle
            player.brain.tracker.lookToAngle(-1 * angle)
            if DEBUG_PENALTY_STATES and CHECK_VALS_EACH_PAN:
                print "Looking to my left! Angle:", otherAngle
                print "I've seen an inside L-Corner", afterPenalty.innerLCornerRight, "outer L-Corner", afterPenalty.outerLCornerRight, "times, T-Corner", afterPenalty.tCornerLeft, "times"
                print "Cumulative horizon distance on this side is", afterPenalty.rightHorizSum

        afterPenalty.lookRight = not afterPenalty.lookRight
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

    if afterPenalty.frameCount > 200:

        # arbitrary thresholds for now!
        if afterPenalty.tCornerLeft > 20 or afterPenalty.outerLCornerRight > 20:
            print "Player thinks the goal is on the right!"
            afterPenalty.decidedSide = True

        elif afterPenalty.tCornerLeft < -20 or afterPenalty.outerLCornerRight < -20:
            print "Player thinks the goal is on the left!"
            afterPenalty.decidedSide = True
            afterPenalty.right = False

    if afterPenalty.frameCount > 300 or afterPenalty.decidedSide:
        if afterPenalty.decidedSide:
            player.brain.resetLocalizationFromPenalty(afterPenalty.right)

        else:
            print("THRESHOLDS WERE NOT MET! Rely on Horizons!")
            if DEBUG_PENALTY_STATES:
                # TODO test if horizons are actually more reliable.
                # See if this helps if we're blocking corners
                afterPenalty.averageLeftHorizon = afterPenalty.leftHorizSum / (afterPenalty.numOfPans / 2.0)
                afterPenalty.averageRightHorizon = afterPenalty.rightHorizSum / (afterPenalty.numOfPans / 2.0)
                print("computed avg left horizon", afterPenalty.averageLeftHorizon, "avg right horizon", afterPenalty.averageRightHorizon)
            player.brain.resetLocalizationFromPenalty(afterPenalty.averageLeftHorizon > afterPenalty.averageRightHorizon)

        if DEBUG_PENALTY_STATES:
            print ("\n-------------------------------------------------------------")
            print("COUNTER TOTALS: ")
            print ("innerLCornerRight:", afterPenalty.innerLCornerRight, "outerLCornerRight:", afterPenalty.outerLCornerRight, "tCornerLeft:", afterPenalty.tCornerLeft)
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

    Based check for if we were placed at kickoff position on the afterPenalty checks above.

    """

    if player.firstFrame():
        print "Going into after penalty from ready or set; am I being manually placed?"
        player.wasPenalized = False

        manualPlacement.frameCounter = 0
        manualPlacement.atKickoffPos = False
        
        manualPlacement.tCornerLeft = 0
        manualPlacement.tCornerRight = 0
        manualPlacement.centerCircle = 0

        manualPlacement.lookRight = True
        manualPlacement.lookForward = False

        manualPlacement.leftHorizon = 0
        manualPlacement.rightHorizon = 0

        player.brain.tracker.lookToAngle(-1 * angle)

        """
        This is a dumb hack.
        It's five days until competition and I don't want to spend my time
        making a new protobuf to handle loc reset requests that take multiple
        points. This code will hopefully send a message to the loc system that
        I can catch once I'm there. This will reset the particle filter to
        evenly spread particles among various coordinates the robot could be
        after they've been manually placed. Those points are defined in
        LocalizationModule.cpp.
        Sorry.
        """
        # print "resetting loc to (999, 999, 999)"
        player.brain.resetLocTo(999, 999, 999)

    manualPlacement.frameCounter += 1
    vis = player.brain.vision

    if manualPlacement.frameCounter % 50 == 0:
        if manualPlacement.lookRight:
            player.brain.tracker.lookToAngle(angle)
        else:
            player.brain.tracker.lookToAngle(-1 * angle)
        manualPlacement.lookRight = not manualPlacement.lookRight

    if player.brain.tracker.isStopped():
        if manualPlacement.lookRight:
            manualPlacement.rightHorizon += vis.horizon_dist
        else:
            manualPlacement.leftHorizon += vis.horizon_dist

    if manualPlacement.frameCounter > 200:
        print("Horizon totals RIGHT:", manualPlacement.rightHorizon, "LEFT:", manualPlacement.leftHorizon)
        return player.goNow('gamePenalized')


    # if player.brain.interface.vision.circle.on  and DEBUG_MANUAL_PLACEMENT:
    #     print "I can see the center circle!"

    # if player.brain.tracker.isStopped() and player.brain.interface.vision.circle.on:
    #     manualPlacement.centerCircle += 1

    # if manualPlacement.frameCounter % 50 == 0:
    #     if manualPlacement.lookRight:
    #         player.brain.tracker.lookToAngle(manualPlacement_angle)
    #         if DEBUG_MANUAL_PLACEMENT:
    #             print("Looking right!")
    #         manualPlacement.lookRight = not manualPlacement.lookRight
    #     elif manualPlacement.lookForward:
    #         player.brain.tracker.lookToAngle(0)
    #         if DEBUG_MANUAL_PLACEMENT:
    #             print("Looking forward!")
    #     else:
    #         player.brain.tracker.lookToAngle(-1 * manualPlacement_angle)
    #         manualPlacement.lookForward = True
    #         if DEBUG_MANUAL_PLACEMENT:
    #             print("Looking left!")

    # if player.brain.tracker.isStopped():
    #     if manualPlacement.lookRight:
    #         for i in range(0, vis.corner_size()):
    #             corner = vis.corner(i)  
    #             if corner.id == 2:
    #                 manualPlacement.tCornerRight += 1
    #     elif manualPlacement.lookForward:
    #         if player.brain.interface.vision.circle.on:
    #             manualPlacement.centerCircle += 1
    #     else:
    #         for i in range(0, vis.corner_size()):
    #             corner = vis.corner(i)
    #             if corner.id == 2:
    #                 manualPlacement.tCornerLeft += 1

    # if manualPlacement.frameCounter > 150:
    #     if DEBUG_MANUAL_PLACEMENT:
    #         if manualPlacement.tCornerLeft > 0 and manualPlacement.tCornerRight > 0 and abs(manualPlacement.tCornerLeft - manualPlacement.tCornerRight) < 5:
    #             print("Player thinks they're near the center line!")
    #         if manualPlacement.centerCircle > 5:
    #             print("Player saw the center circle in front at least 5 times!")
    #     if abs(manualPlacement.tCornerLeft - manualPlacement.tCornerRight) < 5 and manualPlacement.centerCircle > 5:
    #         manualPlacement.atKickoffPos = True

    # if manualPlacement.atKickoffPos or manualPlacement.frameCounter > 150:
    #     if DEBUG_MANUAL_PLACEMENT:
    #         print("TOTAL COUNT")
    #         print("Total count for right T-Corner", manualPlacement.tCornerRight)
    #         print("Total count for left T-Corner", manualPlacement.tCornerLeft)
    #         print("Saw center circle", manualPlacement.centerCircle, "times.")
    #         return player.goNow('gamePenalized')
    #     # reset loc
    #     return player.goLater(player.gameState)

    return player.stay()

@superstate('gameControllerResponder')
def overeagerWhistle(player):
    player.brain.tracker.lookToAngle(0)
    return player.goNow('gamePlaying')

@superState('gameControllerResponder')
def walkOut(player):
    player.brain.nav.walk(0.2, 0, 0)

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

