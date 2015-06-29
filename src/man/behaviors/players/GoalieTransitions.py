#
# The transitions for the goalie for the goalie states.
# Covers chase, position and save. Most of the counters
# for transitions are contained in these transitions.
#

import GoalieConstants as constants
import noggin_constants as field
import math
import VisualGoalieStates
import GoalieStates
from objects import RelRobotLocation

# New vision system visual goalie
def getLines(player):
    visionLines = player.brain.visionLines
    # Add in checks to not add repeat lines?

    for i in range(0, visionLines.line_size()):
        GoalieStates.watchWithLineChecks.lines.append(visionLines.line(i).inner)

    if len(GoalieStates.watchWithLineChecks.lines) > constants.MEM_THRESH:
        GoalieStates.watchWithLineChecks.lines = GoalieStates.watchWithLineChecks.lines[1:]

        # r = line.inner.r
        # t = line.inner.t
        # x0 = r * math.cos(t)
        # y0 = r * math.sin(t)
        # x1 = x0 + line.inner.eP0 * math.sin(t)
        # y1 = y0 + -line.inner.eP0 * math.cos(t)
        # x2 = x0 + line.inner.eP1 * math.sin(t)
        # y2 = y0 + -line.inner.eP1 * math.cos(t)

def findCorner(player):
    visionLines = player.brain.visionLines

    # for i in range(0, visionLines.line_size()):


# def getFrontLine(player):
#     # Assume facing forward
#     for line in GoalieStates.watchWithLineChecks.lines:
#         r = line.r
#         t = line.t
#         # Assumptions: facing forward
#         # If have good t value and bad r value, reposition accordingly
#         if math.fabs(t - constants.EXPECTED_FRONT_LINE_T) < constants.T_THRESH \
#         and math.fabs(r - constants.EXPECTED_FRONT_LINE_R) < constants.R_THRESH:
#             return line

def frontLineCheckShouldReposition(player):
    getLines(player)
    x_dest = 0.0
    y_dest = 0.0
    h_dest = 0.0

    if not GoalieStates.watchWithLineChecks.correctFacing:
        return False

    for line in GoalieStates.watchWithLineChecks.lines:
        r = line.r
        t = math.degrees(line.t)

        # Assumptions: facing forward
        # If we find a line that, judging by its t value, is likely the front line of
        # the goalbox, use the r value to correct the robot's y position
        # If have good t value and bad r value, reposition accordingly
        # Additional r < 100 check to throw away the middle line
        if math.fabs(t - constants.EXPECTED_FRONT_LINE_T) < constants.T_THRESH \
        and math.fabs(r - constants.EXPECTED_FRONT_LINE_R) > constants.R_THRESH \
        and r < 100.0:
            x_dest = r - constants.EXPECTED_FRONT_LINE_R
            print "Front was TRUE"
            print x_dest
            print r
            player.homeDirections += [RelRobotLocation(x_dest, y_dest, h_dest)]
            return True

    if len(player.homeDirections) > constants.BUFFER_THRESH:
        player.homeDirections = player.homeDirections[1:]

    return False


def sideLineCheckShouldReposition(player):
    x_dest = 0.0
    y_dest = 0.0
    h_dest = 0.0

    if not GoalieStates.watchWithLineChecks.correctFacing:
        return False

    for line in GoalieStates.watchWithLineChecks.lines:
        r = line.r
        t = math.degrees(line.t)
        # Assumptions: facing forward
        # If we find a line that, judging by its t value, is likely the right line,
        # of the goalbox, use the r value to correct the robot's x position
        # Additional r < 200 check to throw away the side field lines
        if math.fabs(t - constants.EXPECTED_RIGHT_LINE_T) < constants.T_THRESH \
        and math.fabs(r - constants.EXPECTED_SIDE_LINE_R) > constants.R_THRESH \
        and r < 170.0:
            y_dest = constants.EXPECTED_SIDE_LINE_R - r
            print "Right side was TRUE"
            print ("ydest: ", y_dest)
            print ("r: ",r)
            player.homeDirections += [RelRobotLocation(x_dest, y_dest, h_dest)]
            return True

        # Assumptions: facing forward
        # Same as above, except with the left side line
        if math.fabs(t - constants.EXPECTED_LEFT_LINE_T) < constants.T_THRESH \
        and math.fabs(r - constants.EXPECTED_SIDE_LINE_R) > constants.R_THRESH \
        and r < 170.0:
            y_dest = r - constants.EXPECTED_SIDE_LINE_R
            print "Left side was TRUE"
            print ("ydest: ", y_dest)
            print ("r: ",r)
            player.homeDirections += [RelRobotLocation(x_dest, y_dest, h_dest)]
            return True

    return False

def shouldTurn(player):
    if GoalieStates.watchWithLineChecks.numTurns > 1 \
    and GoalieStates.watchWithLineChecks.numFixes < 2:
        return False
    if GoalieStates.watchWithLineChecks.numTurns == 2:
        return False

    h_dest = 0.0

    longestLine = None
    longestLength = 0.0

    for line in GoalieStates.watchWithLineChecks.lines:
        r = line.r
        t = math.degrees(line.t)
        length = getLineLength(line)

        # Try to avoid facing a sideline
        if length < 30.0 and r > 30.0:
            continue

        # Avoid invalid lines, lines that are too far, and lines that are possibly
        # the left and right sidelines
        if math.fabs(t - constants.EXPECTED_RIGHT_LINE_T) < constants.T_THRESH \
        or math.fabs(t - constants.EXPECTED_LEFT_LINE_T) < constants.T_THRESH \
        or r > 100.0 or t == 0.0:
            continue

        # Fix this.. very hacky: basically return that we DON'T need to turn
        # if we see a reasonable front line
        if math.fabs(t - constants.EXPECTED_FRONT_LINE_T) < constants.T_THRESH:
            h_dest = 0.0
            player.homeDirections += [RelRobotLocation(0.0, 0.0, h_dest)]
            print ("t: ", t)
            print ("r: ", r)
            return True

        # Assumptions: not facing forward....?
        # Hopefully will find a line close by (r < 100) with an unusual t value
        # and use the information to help correct itself
        # Theoretically will usually be the long front line?
        else:
            if length > longestLength:
                longestLine = line
                longestLength = length
            # h_dest = t - 90.0
            # player.homeDirections += [RelRobotLocation(0.0, 0.0, h_dest)]
            # print "Should turn was TRUE"
            # print ("hdest: ", h_dest)
            # print ("t was: ", t)
            # print ("r was:", r)
            # return True

        if longestLine is not None:
            h_dest = t - 90.0
            player.homeDirections += [RelRobotLocation(0.0, 0.0, h_dest)]
            print "Should turn was TRUE"
            print ("hdest: ", h_dest)
            print ("t was: ", t)
            print ("r was:", r)
            return True

    return False

def shouldGoForward(player):
    # If we see two parallel lines in front of us, we're probably in the goal...
    lines = player.brain.visionLines

    for i in range(0, player.brain.visionLines.line_size()):
        r = lines.line(i).inner.r
        t = math.degrees(lines.line(i).inner.t)
        for l in range(0, player.brain.visionLines.line_size()):
            r2 = lines.line(l).inner.r
            t2 = math.degrees(lines.line(l).inner.t)

            if (l != i) and math.fabs(t - t2) < 6.0 \
            and r2 < 120.0 and r < 120.0 and math.fabs(r - r2) > 35.0 \
            and r != 0.0 and r2 != 0.0:
                print "I'm seeing two lines, I should go forward"
                print ("r1: ", r, "r2: ", r2, " t1: ", t, "t2: ", t2)
                player.homeDirections += [RelRobotLocation(25.0, 0.0, 0.0)]
                return True

    return False


def shouldBackUp(player):
    # If we cannot see any good lines, should probably back up!

    for line in GoalieStates.watchWithLineChecks.lines:
        r = line.r
        t = math.degrees(line.t)

        if r < 220.0 and r != 0.0:
            return False

    if player.brain.visionLines.line_size() == 0:
        print "My brain sees no lines right now"
        return True

    print "Couldn't find any good lines, backup TRUE"
    return True

def shouldStopGoingBack(player):
    visionLines = player.brain.visionLines
    # Add in checks to not add repeat lines?

    for i in range(0, visionLines.line_size()):
        r = visionLines.line(i).inner.r
        t = math.degrees(visionLines.line(i).inner.t)

        if r != 0.0 and r < 40.0 and r > 25.0 and t < 130.0 and t > 50.0:
            print "I see a line now! I should probably stop going backwards"
            print ("r: ", r)
            print ("t: ", t)
            return True

    return False

def getBearingFromRobot(x, y):
    return math.degrees(math.atan2(x, -y));

def getLineLength(line):
    r = line.r
    t = line.t
    x0 = r * math.cos(t)
    y0 = r * math.sin(t)
    x1 = x0 + line.ep0 * math.sin(t)
    y1 = y0 + -line.ep0 * math.cos(t)
    x2 = x0 + line.ep1 * math.sin(t)
    y2 = y0 + -line.ep1 * math.cos(t)

    x = x2 - x1
    y = y2 - y1
    return math.sqrt(x*x + y*y)

def shouldPositionRight(player):
    if player.brain.ball.bearing_deg < -40.0 and \
    player.inPosition is not constants.RIGHT_POSITION and \
    player.inPosition is not constants.NOT_IN_POSITION:
        player.homeDirections += [RelRobotLocation(5.0, -30.0, 0.0)]
        GoalieStates.watchWithLineChecks.shiftedPosition = True
        return True

    GoalieStates.watchWithLineChecks.shiftedPosition = False
    return False

def shouldPositionLeft(player):
    if player.brain.ball.bearing_deg > 40.0 and \
    player.inPosition is not constants.LEFT_POSITION and \
    player.inPosition is not constants.NOT_IN_POSITION:
        player.homeDirections += [RelRobotLocation(5.0, 30.0, 0.0)]
        GoalieStates.watchWithLineChecks.shiftedPosition = True
        return True

    GoalieStates.watchWithLineChecks.shiftedPosition = False
    return False

def shouldStopTurning(player):
    lines = player.brain.visionLines

    for i in range(0, lines.line_size()):
        r = lines.line(i).inner.r
        t = math.degrees(lines.line(i).inner.t)
        if math.fabs(t - 90.0) < 15.0 and r is not 0.0 and r < 40.0:
            print "I see a line! I'm assuming its part of the goalbox and I'm going there"
            print ("R:", r, "T:", t)
            player.homeDirections += [RelRobotLocation(r+35.0, 0.0, 0.0)]
            return True
        if math.fabs(t - 90.0) < 15.0 and r is not 0.0 and r > 110.0:
            print "I see a line! Its decently far, so I'm assuming its the back of the goalbox"
            player.homeDirections += [RelRobotLocation(r-25.0, 0.0, 0.0)]
            print ("R:", r, "T:", t)
            return True

    return False









# Visual Goalie

def getLeftGoalboxCorner(player):
    vision = player.brain.interface.visionField

    # check top corners first
    for i in range(0, vision.visual_corner_size()):
        for j in range(0, vision.visual_corner(i).poss_id_size()):
            if (vision.visual_corner(i).poss_id(j) ==
                vision.visual_corner(i).corner_id.YELLOW_GOAL_LEFT_L):
                if (vision.visual_corner(i).orientation < 0):
                    return vision.visual_corner(i)

    # also look in bottom camera
    for i in range(0, vision.bottom_corner_size()):
        for j in range(0, vision.bottom_corner(i).poss_id_size()):
            if (vision.bottom_corner(i).poss_id(j) ==
                vision.bottom_corner(i).corner_id.YELLOW_GOAL_LEFT_L):
                # HACK
                if (vision.bottom_corner(i).orientation > 0):
                    return vision.bottom_corner(i)

    return None

def getRightGoalboxCorner(player):
    vision = player.brain.interface.visionField

    # check top corners first
    for i in range(0, vision.visual_corner_size()):
        for j in range(0, vision.visual_corner(i).poss_id_size()):
            if (vision.visual_corner(i).poss_id(j) ==
                vision.visual_corner(i).corner_id.YELLOW_GOAL_RIGHT_L):
                if (vision.visual_corner(i).orientation > 0):
                    return vision.visual_corner(i)

    # also look in bottom camera
    for i in range(0, vision.bottom_corner_size()):
        for j in range(0, vision.bottom_corner(i).poss_id_size()):
            if (vision.bottom_corner(i).poss_id(j) ==
                vision.bottom_corner(i).corner_id.YELLOW_GOAL_RIGHT_L):
                # HACK
                if (vision.bottom_corner(i).orientation < 0):
                    return vision.bottom_corner(i)

    return None

# Rel X of corner from robot
def getCornerRelX(alpha, corner):
    return (corner.visual_detection.distance *
            math.cos(corner.physical_orientation + math.radians(alpha)))

# Rel Y of corner from robot
def getCornerRelY(alpha, corner):
    return (corner.visual_detection.distance *
            math.sin(corner.physical_orientation + math.radians(alpha)))

def getRobotGlobalHeading(alpha, corner):
    return math.degrees(corner.physical_orientation + math.radians(alpha) -
                        corner.visual_detection.bearing)



def badLeftCornerObservation(player):
    corner = getLeftGoalboxCorner(player)
    if not corner:
        return False

    dDist = math.fabs(constants.EXPECTED_CORNER_DIST_FROM_CENTER -
                      corner.visual_detection.distance)
    dBear = math.fabs(constants.EXPECTED_LEFT_CORNER_BEARING_FROM_CENTER -
                      corner.visual_detection.bearing_deg)

    if not (dDist > constants.CORNER_DISTANCE_THRESH or
            dBear > constants.CORNER_BEARING_THRESH):
        return False

    homeRelX = -(field.GOALBOX_DEPTH - getCornerRelX(90, corner) -
                 constants.GOALIE_OFFSET)
    homeRelY = -(field.GOALBOX_WIDTH/2.0 - getCornerRelY(90, corner))
    homeRelH = -getRobotGlobalHeading(90, corner)

    player.homeDirections += [RelRobotLocation(homeRelX,
                                               homeRelY,
                                               homeRelH)]


    if len(player.homeDirections) > constants.BUFFER_THRESH:
        player.homeDirections = player.homeDirections[1:]

    return True

def badRightCornerObservation(player):
    corner = getRightGoalboxCorner(player)
    if not corner:
        return False

    dDist = math.fabs(constants.EXPECTED_CORNER_DIST_FROM_CENTER -
                      corner.visual_detection.distance)
    dBear = math.fabs(constants.EXPECTED_RIGHT_CORNER_BEARING_FROM_CENTER -
                      corner.visual_detection.bearing_deg)

    if not (dDist > constants.CORNER_DISTANCE_THRESH or
            dBear > constants.CORNER_BEARING_THRESH):
        return False

    homeRelX = -(field.GOALBOX_DEPTH - getCornerRelX(0, corner) -
                 constants.GOALIE_OFFSET)
    homeRelY = field.GOALBOX_WIDTH/2.0 + getCornerRelY(0, corner)
    homeRelH = -getRobotGlobalHeading(0, corner)

    player.homeDirections += [RelRobotLocation(homeRelX,
                                               homeRelY,
                                               homeRelH)]

    if len(player.homeDirections) > constants.BUFFER_THRESH:
        player.homeDirections = player.homeDirections[1:]

    return True

def goodLeftCornerObservation(player):
    if player.counter < 60:
        return False

    corner = getLeftGoalboxCorner(player)
    if not corner:
        return False

    dDist = math.fabs(constants.EXPECTED_CORNER_DIST_FROM_CENTER -
                      corner.visual_detection.distance)
    dBear = math.fabs(constants.EXPECTED_LEFT_CORNER_BEARING_FROM_CENTER -
                      corner.visual_detection.bearing_deg)

    if (dDist < constants.CORNER_DISTANCE_THRESH + 10.0 and
        dBear < constants.CORNER_BEARING_THRESH + 10.0):
        return True

    return False

def goodRightCornerObservation(player):
    if player.counter < 60:
        return False

    corner = getRightGoalboxCorner(player)
    if not corner:
        return False

    dDist = math.fabs(constants.EXPECTED_CORNER_DIST_FROM_CENTER -
                      corner.visual_detection.distance)
    dBear = math.fabs(constants.EXPECTED_RIGHT_CORNER_BEARING_FROM_CENTER -
                      corner.visual_detection.bearing_deg)

    if (dDist < constants.CORNER_DISTANCE_THRESH + 10.0 and
        dBear < constants.CORNER_BEARING_THRESH + 10.0):
        return True

    return False

def shouldMoveForward(player):
    vision = player.brain.interface.visionField

    if (player.counter > 150 and
        ((player.brain.yglp.on and
          math.fabs(player.brain.yglp.bearing) < 80 and
          player.brain.yglp.distance < 300.0 and
          player.brain.yglp.distance != 0.0) or
         (player.brain.ygrp.on and
          math.fabs(player.brain.ygrp.bearing) < 80 and
          player.brain.yglp.distance < 300.0 and
          player.brain.yglp.distance != 0.0))):
        return True

    foundGoalBoxTop = 0
    orientation = 0

    for i in range(0, vision.visual_line_size()):
        if vision.visual_line(i).visual_detection.distance < 200.0:
            foundGoalBoxTop = vision.visual_line(i).visual_detection.distance
            orientation = vision.visual_line(i).angle
            break

    if not foundGoalBoxTop:
        return False

    for i in range(0, vision.bottom_line_size()):
        if (vision.bottom_line(i).visual_detection.distance < 70.0 and
            vision.visual_field_edge.distance_m > 150.0 and
            math.fabs(vision.bottom_line(i).visual_detection.distance -
                      foundGoalBoxTop) > 30.0 and
            math.fabs(math.degrees(vision.bottom_line(i).angle -
                                   orientation)) < 45.0):
            return True

    return False

def shouldMoveBackwards(player):
    vision = player.brain.interface.visionField

    if (vision.bottom_line_size() == 0 and
        vision.visual_line_size() == 0):
        return True

    for i in range(0, vision.visual_line_size()):
        if vision.visual_line(i).visual_detection.distance < 120.0:
            return False

    for i in range(0, vision.bottom_line_size()):
        if vision.bottom_line(i).visual_detection.distance < 120.0:
            return False

    return True

def facingSideways(player):
    """
    If the robot is facing a post directly, it's probably turned around.
    """
    if ((player.brain.yglp.on and
         math.fabs(player.brain.yglp.bearing_deg) < 20.0 and
         player.brain.yglp.bearing_deg != 0.0 and
         player.brain.yglp.distance < 300.0) or
        (player.brain.ygrp.on and
         math.fabs(player.brain.ygrp.bearing_deg) < 20.0 and
         player.brain.ygrp.bearing_deg != 0.0 and
         player.brain.ygrp.distance < 300.0)):
        return True
    else:
        return False

def facingBackwards(player):
    return player.brain.interface.visionField.visual_field_edge.distance_m < 110.0

def shouldReposition(player):
    return (badLeftCornerObservation(player) or
            badRightCornerObservation(player))

def goodPosition(player):
    return (goodLeftCornerObservation(player) or
            goodRightCornerObservation(player))

def atGoalArea(player):
    """
    Checks if robot is close enough to the field edge to be at the goal.
    """
    #magic number
    vision = player.brain.interface.visionField
    return ((vision.visual_field_edge.distance_m < 110.0
             and vision.visual_field_edge.distance_m != 0.0)
            or (player.brain.yglp.distance < 20.0
                and player.brain.yglp.on
                and not player.brain.yglp.distance == 0.0)
            or (player.brain.ygrp.distance < 20.0
                and player.brain.ygrp.on
                and not player.brain.ygrp.distance == 0.0))

def facingForward(player):
    """
    Checks if a robot is facing the cross, which is more or less forward
    if it is in the goal.
    """
    #magic numbers
    vision = player.brain.interface.visionField
    return ((vision.visual_field_edge.distance_m > 800.0 and
             vision.visual_field_edge.distance_m != 1000.0) or
            (math.fabs(vision.visual_cross.bearing) < 10.0 and
             vision.visual_cross.distance > 0.0))

def facingBall(player):
    """
    Checks if the ball is right in front of it.
    """
    #magic numbers
    return (math.fabs(player.brain.ball.bearing_deg) < 10.0 and
            player.brain.ball.vis.on)

def goodToBookIt(player):
    vision = player.brain.interface.visionField

    return (vision.visual_field_edge.distance_m < 300.0 and
            ((not player.brain.yglp.on) or
             math.fabs(player.brain.yglp.bearing_deg) > 40.0) and
            ((not player.brain.ygrp.on) or
             math.fabs(player.brain.ygrp.bearing_deg) > 40.0))

def safelyIllegal(player):
    return player.brain.interface.visionField.visual_field_edge.distance_m < 30.0

def notTurnedAround(player):
    """
    Checks that we are actually facing the field when returning from
    penalty.
    """
    return (player.brain.interface.visionField.visual_field_edge.distance_m
            > 400.0)

# Saving transitions....
def shouldDiveRight(player):
    if player.firstFrame():
        shouldDiveRight.lastFramesOff = 21

    sightOk = True
    ball = player.brain.ball

    if shouldDiveRight.lastFramesOff > 20 and ball.vis.frames_on < 20:
        sightOk = False

    if not ball.vis.on:
        shouldDiveRight.lastFramesOff = ball.vis.frames_off

    return (ball.mov_vel_x < -6.0 and
            ball.mov_speed > 8.0 and
            ball.rel_y_intersect_dest < -20.0 and
            ball.distance < 150.0 and
            sightOk)

def shouldDiveLeft(player):
    if player.firstFrame():
        shouldDiveLeft.lastFramesOff = 21

    sightOk = True
    ball = player.brain.ball

    if shouldDiveLeft.lastFramesOff > 20 and ball.vis.frames_on < 20:
        sightOk = False

    if not ball.vis.on:
        shouldDiveLeft.lastFramesOff = ball.vis.frames_off

    return (ball.mov_vel_x < -6.0 and
            ball.mov_speed > 8.0 and
            ball.rel_y_intersect_dest > 20.0 and
            ball.distance < 150.0 and
            sightOk)

def shouldSquat(player):
    if player.firstFrame():
        shouldSquat.lastFramesOff = 21

    sightOk = True
    ball = player.brain.ball

    if shouldSquat.lastFramesOff > 20 and ball.vis.frames_on < 20:
        sightOk = False

    if not ball.vis.on:
        shouldSquat.lastFramesOff = ball.vis.frames_off

    return (ball.mov_vel_x < -4.0 and
            ball.mov_speed > 8.0 and
            abs(ball.rel_y_intersect_dest) < 40.0 and
            ball.distance < 150.0 and
            sightOk)

def shouldClearDangerousBall(player):
    # ball must be visible
    if player.brain.ball.vis.frames_off > 10:
        return False

    if (math.fabs(player.brain.ball.bearing_deg) > 70.0 and
        player.brain.ball.distance < 50.0):
        if player.brain.ball.bearing_deg < 0:
            VisualGoalieStates.clearIt.dangerousSide = constants.RIGHT
        else:
            VisualGoalieStates.clearIt.dangerousSide = constants.LEFT
        return True

    return False

def ballSafe(player):
    return math.fabs(player.brain.ball.bearing_deg) < 60.0

def shouldClearBall(player):
    """
    Checks that the ball is more or less in the goal box.
    """
    # less than 1.5 minutes left or winning/losing badly
    shouldBeAggressive = (player.brain.game.secs_remaining < 90 or
                          (abs(player.brain.game.team(0).score -
                               player.brain.game.team(1).score) > 1))

    if shouldBeAggressive and not player.aggressive:
        print "The goalie is now AGGRESSIVE"
        player.aggressive = True
    elif not shouldBeAggressive and player.aggressive:
        print "The goalie is no longer AGGRESSIVE"
        player.aggressive = False

    # ball must be visible
    if not player.brain.ball.vis.on:
        return False

    shouldGo = False

    # if definitely within good chasing area
    if (player.brain.ball.distance < 120.0):
        walkedTooFar.xThresh = 150.0
        walkedTooFar.yThresh = 150.0
        shouldGo = True

    # farther out but being aggressive
    if (player.brain.ball.distance < 150.0 and
        player.brain.ball.is_stationary and
        player.aggressive):
        walkedTooFar.xThresh = 180.0
        walkedTooFar.yThresh = 180.0
        shouldGo = True

    # to goalie's sides, being aggressive
    if (math.fabs(player.brain.ball.bearing_deg) > 50.0 and
        player.aggressive):
        walkedTooFar.xThresh = 300.0
        walkedTooFar.yThresh = 300.0
        shouldGo = True

    #TODO: implement check to see if ball is outside of goalbox or not!

    if shouldGo:
        if player.brain.ball.bearing_deg < -50.0:
            VisualGoalieStates.clearIt.dangerousSide = constants.RIGHT
        elif player.brain.ball.bearing_deg > 50.0:
            VisualGoalieStates.clearIt.dangerousSide = constants.LEFT
        else:
            VisualGoalieStates.clearIt.dangerousSide = -1

        if player.brain.ball.bearing_deg < 0.0:
            VisualGoalieStates.clearIt.ballSide = constants.RIGHT
        else:
            VisualGoalieStates.clearIt.ballSide = constants.LEFT

        lines = player.brain.visionLines
        for i in range(0, lines.line_size()):
            r = lines.line(i).inner.r
            t = math.degrees(lines.line(i).inner.t)
            if r < 120.0 and r is not 0.0:
                if player.brain.ball.distance > r:
                    VisualGoalieStates.clearIt.inGoalbox = False
                    print "I think the ball is outside the goalbox!"
                    print ("R:", r)
                else:
                    VisualGoalieStates.clearIt.inGoalbox = True
                    print "I think the ball is in the goalbox"

    return shouldGo

def ballLostStopChasing(player):
    """
    If the robot does not see the ball while chasing, it is lost. Delay
    in case our shoulder pads are just hiding it.
    """
    if player.brain.ball.vis.frames_off > 100:
        return True

def ballMovedStopChasing(player):
    """
    If the robot has been chasing for a while and it is far away, it should
    stop chasing.
    """
    return (player.brain.ball.distance > 130.0 and
            player.counter > 100.0)

def walkedTooFar(player):
    # for the odometry reset delay
    if player.counter < 3:
        return False

    if player.aggressive:
        return False

    return (player.brain.interface.odometry.x > walkedTooFar.xThresh or
            math.fabs(player.brain.interface.odometry.y) > walkedTooFar.yThresh)

def reachedMyDestination(player):
    """
    The robot has reached the ball after walking to it.
    """
    return player.brain.nav.isAtPosition()

def doneWalking(player):
    """
    HACK for walkTo from nav, which does not always switch to done.
    """
    return player.brain.nav.currentState == 'standing'

def successfulKick(player):
    return player.counter > 50 and VisualGoalieStates.clearIt.dangerousSide == -1

def successfulKickAndTurn(player):
    # I went to the far side of a goal and it will be hard to get back!
    if VisualGoalieStates.clearIt.dangerousSide == -1:
        return False

    if VisualGoalieStates.clearIt.dangerousSide == constants.RIGHT:
        player.homeDirections += [RelRobotLocation(-50.0, 0.0, 15.0)]
    else:
        player.homeDirections += [RelRobotLocation(-50.0, 0.0, -15.0)]
    print "Here I go! It was very far to the side"

    # player.homeDirections = player.homeDirections[1:]
    return player.counter > 30

def whiffed(player):
    """
    If the ball is just sitting at the goalie's feet after kicking, it
    should try again.
    """
    return (player.brain.ball.rel_x < 40.0 and
            math.fabs(player.brain.ball.rel_y) < 25.0 and
            player.brain.ball.vis.on)

def saveNow(player):
    if player.brain.ball.mov_rel_y < -6.0:
        GoalieStates.doDive.side = constants.RIGHT
    elif player.brain.ball.mov_rel_y > 6.0:
        GoalieStates.doDive.side = constants.LEFT
    else:
        GoalieStates.doDive.side = 3

    return player.brain.ball.mov_speed > 8.0
