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

def getCorners(player):
    corners = player.brain.visionCorners

    # print ("horizon dist:", player.brain.vision.horizon_dist)
    # print ("num corners:", player.brain.visionCorners.corner_size())

    # for k in range(0, player.brain.visionCorners.corner_size()):
    #     c = player.brain.visionCorners
    #     print("x", c.corner(k).x, "y", c.corner(k).y)
    #     print("id", c.corner(k).id)
    # print " -------------------------"

  # Concave,
  # Convex,
  # T,

# New vision system visual goalie
def getLines(player):
    visionLines = player.brain.visionLines
    # Add in checks to not add repeat lines?

    for i in range(0, visionLines.line_size()):
        GoalieStates.watchWithLineChecks.lines.append(visionLines.line(i).inner)

    if len(GoalieStates.watchWithLineChecks.lines) > constants.MEM_THRESH:
        GoalieStates.watchWithLineChecks.lines = GoalieStates.watchWithLineChecks.lines[3:]

    if len(player.homeDirections) > constants.BUFFER_THRESH:
        player.homeDirections = [player.homeDirections[-1]]

        # r = line.inner.r
        # t = line.inner.t
        # x0 = r * math.cos(t)
        # y0 = r * math.sin(t)
        # x1 = x0 + line.inner.ep0 * math.sin(t)
        # y1 = y0 + -line.inner.ep0 * math.cos(t)
        # x2 = x0 + line.inner.ep1 * math.sin(t)
        # y2 = y0 + -line.inner.ep1 * math.cos(t)

def frontLineCheckShouldReposition(player):
    # getLines(player)
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
        if (math.fabs(t - constants.EXPECTED_FRONT_LINE_T) < constants.T_THRESH \
        or math.fabs(t - constants.EXPECTED_FRONT_LINE_T_2) < constants.T_THRESH) \
        and math.fabs(r - constants.EXPECTED_FRONT_LINE_R) > constants.R_THRESH \
        and r < 100.0 and r != 0.0:
            x_dest = r - constants.EXPECTED_FRONT_LINE_R
            print "Front was TRUE"
            print x_dest
            print r
            player.homeDirections += [RelRobotLocation(x_dest, y_dest, h_dest)]
            return True

    return False

def facingBackward(player):
    if player.brain.vision.horizon_dist < 200.0 and\
    math.fabs(math.degrees(player.brain.interface.joints.head_yaw)) < 15.0:
        print("I'm FACing backWARDS! yaw:", math.degrees(player.brain.interface.joints.head_yaw))
        player.homeDirections = []
        player.homeDirections += [RelRobotLocation(0, 0, 180.0)]
        return True
    return False

#TODO Finish
def seeGoalbox(player):
    lines = player.brain.visionLines
    for i in range(0, lines.line_size()):
        r1 = lines.line(i).inner.r
        t1 = math.degrees(lines.line(i).inner.t)

        for l in range(0, player.brain.visionLines.line_size()):
            r2 = lines.line(l).inner.r
            t2 = math.degrees(lines.line(l).inner.t)

        if l != i and math.fabs(t1 - t2) < 10.0 and r2 < 120.0 and r1 < 120.0 \
        and r1 != 0.0 and r2 != 0.0 and math.fabs(r1 - r2) > 15:
            print("I think I see the goalbox!")
            print("r1, t1:", r1, t1, "r2,t2", r2, t2)

            if r1 > r2:
                backline = lines.line(i).inner
                frontline = lines.line(l).inner
            else:
                backline = lines.line(l).inner
                frontline = lines.line(i).inner

            r = backline.r
            t = math.degrees(backline.t)
            x0 = r * math.cos(t)
            y0 = r * math.sin(t)
            x1 = x0 + backline.ep0 * math.sin(t)
            y1 = y0 + -backline.ep0 * math.cos(t)
            x2 = x0 + backline.ep1 * math.sin(t)
            y2 = y0 + -backline.ep1 * math.cos(t)
            mx = (r1+r2)/3 * 2 #(x1+x2)/2
            my = 0 #(y1+y2)/2
            if t > 180 and t < 340:
                h = 20 #(360 - t) / 2
            elif t < 180 and t > 15:
                h = -20 #-(t/2)
            else:
                h = 0
            print("Backline r:", r, "t:", t)
            print("mx:", mx, "my", my, "h", h)
            print("horizon", player.brain.vision.horizon_dist)
            player.homeDirections += [RelRobotLocation(mx, my, h)]
            return True

    return False

def facingSideways(player):
    if player.brain.vision.horizon_dist > 200 and\
    player.brain.vision.horizon_dist < 600 and\
    math.fabs(math.degrees(player.brain.interface.joints.head_yaw)) < 10.0:
        print("I'm FACing sideways! yaw:", math.degrees(player.brain.interface.joints.head_yaw))
        player.homeDirections += [RelRobotLocation(0, 0, 180.0)]
        player.homeDirections = player.homeDirections[1:]
        print("horizon", player.brain.vision.horizon_dist)
        if player.brain.loc.h < 180.0:
            # Facing left
            player.homeDirections = []
            player.homeDirections += [RelRobotLocation(0,0,-70)]
        else:
            # Facing right
            print "I think I'm facing right!!"
            player.homeDirections = []
            player.homeDirections += [RelRobotLocation(0,0,70)]
        return True
    elif math.fabs(math.degrees(player.brain.interface.joints.head_yaw)) < 10.0:
        print "I'm Def NOT facing sideways!!"
        print("horizon", player.brain.visionLines.horizon_dist)
    return False

def facingFront(player):
    if math.fabs(player.brain.vision.horizon_dist - 1000) < 200 and\
    math.fabs(math.degrees(player.brain.interface.joints.head_yaw)) < 10.0:
        print("I think im facingFront yaw:", math.degrees(player.brain.interface.joints.head_yaw))
        print("horizon dist:",player.brain.vision.horizon_dist )
        return True
    return False

def sideLineCheckShouldReposition(player):
    x_dest = 0.0
    y_dest = 0.0
    h_dest = 0.0

    if GoalieStates.watchWithLineChecks.correctFacing is False:
        # print("incorrect facing")
        return False

    # reasonAbleFrontLine = False
    # for line in GoalieStates.watchWithLineChecks.lines:
    #     if math.fabs(math.degrees(line.t) - constants.EXPECTED_FRONT_LINE_T) < 15.0:
    #         if line.r > 15.0:
    #             reasonAbleFrontLine = True

    # if GoalieStates.watchWithLineChecks.numFixes < 1:
    #     # print "not enough fixes"
    #     return False

    for line in GoalieStates.watchWithLineChecks.lines:
        r = line.r
        t = math.degrees(line.t)
        # Assumptions: facing forward
        # If we find a line that, judging by its t value, is likely the right line,
        # of the goalbox, use the r value to correct the robot's x position
        # Additional r < 200 check to throw away the side field lines
        if math.fabs(t - constants.EXPECTED_RIGHT_LINE_T) < constants.T_THRESH \
        and math.fabs(r - constants.EXPECTED_SIDE_LINE_R) > constants.R_THRESH \
        and r < 170.0 and r != 0.0:
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
        and r < 170.0 and r != 0.0:
            y_dest = r - constants.EXPECTED_SIDE_LINE_R
            print "Left side was TRUE"
            print ("ydest: ", y_dest)
            print ("r: ",r)
            player.homeDirections += [RelRobotLocation(x_dest, y_dest, h_dest)]
            return True

    return False

def shouldTurn(player):
    # Turn twice, then reposition, then turn twice again, etc.
    if GoalieStates.watchWithLineChecks.numTurns > 1 \
    and GoalieStates.watchWithLineChecks.numFixes < 1:
        return False
    if GoalieStates.watchWithLineChecks.numTurns >= 4:
        return False

    h_dest = 0.0

    longestLine = None
    longestLength = 0.0

    if facingSideways(player):
        print "I'm facing sideways..."
        print("My loc.h:", player.brain.loc.h)

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
        or r > 100.0 or r == 0.0:
            continue

        # Fix this.. very hacky: basically return that we DON'T need to turn
        # if we see a reasonable front line
        if (math.fabs(t - constants.EXPECTED_FRONT_LINE_T) < constants.T_THRESH\
        or math.fabs(t - constants.EXPECTED_FRONT_LINE_T_2) < constants.T_THRESH) \
        and not GoalieStates.watchWithLineChecks.correctFacing:
            h_dest = 0.0
            player.homeDirections += [RelRobotLocation(0.0, 0.0, h_dest)]
            print ("t: ", t)
            print ("r: ", r)
            print "Should turn was TRUE"
            return True

        # Assumptions: not facing forward...
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
            if t > 180:
                h_dest = t - constants.EXPECTED_FRONT_LINE_T_2
            else:
                h_dest = t - constants.EXPECTED_FRONT_LINE_T

            if h_dest < 10.0:
                return False
            print "Should turn was TRUE"
            print ("hdest: ", h_dest)
            print ("t was: ", t)
            print ("r was:", r)
            print ("length of line:", longestLength)
            if h_dest > 100.0:
                print "Turn too big, not actually doing it!!"
                return False
            player.homeDirections += [RelRobotLocation(0.0, 0.0, h_dest)]
            return True

    return False

def shouldGoForward(player):
    # If we see two parallel lines in front of us, we're probably in the goal...
    # lines = player.brain.visionLines
    lines = GoalieStates.watchWithLineChecks.lines

    for i in range(0, len(lines)):
        r = lines[i].r
        t = math.degrees(lines[i].t)
        for l in range(0, len(lines)):
            r2 = lines[l].r
            t2 = math.degrees(lines[l].t)

            if (l != i) and math.fabs(t - t2) < 15.0 \
            and r2 < 120.0 and r < 120.0 and math.fabs(r - r2) > 25.0 \
            and r != 0.0 and r2 != 0.0:
                print "I'm seeing two lines, I should go forward"
                print ("r1: ", r, "r2: ", r2, " t1: ", t, "t2: ", t2)
                print("horizon", player.brain.vision.horizon_dist)
                player.homeDirections = []
                player.homeDirections += [RelRobotLocation(25.0, 0.0, 0.0)]
                return True

        # r = lines.line(i).inner.r
        # t = math.degrees(lines.line(i).inner.t)
        # for l in range(0, player.brain.visionLines.line_size()):
        #     r2 = lines.line(l).inner.r
        #     t2 = math.degrees(lines.line(l).inner.t)

        #     if (l != i) and math.fabs(t - t2) < 15.0 \
        #     and r2 < 120.0 and r < 120.0 and math.fabs(r - r2) > 25.0 \
        #     and r != 0.0 and r2 != 0.0:
        #         print "I'm seeing two lines, I should go forward"
        #         print ("r1: ", r, "r2: ", r2, " t1: ", t, "t2: ", t2)
        #         player.homeDirections = []
        #         player.homeDirections += [RelRobotLocation(25.0, 0.0, 0.0)]
        #         return True

    return False

def noTopLine(player):
    # getLines(player)
    for line in GoalieStates.watchWithLineChecks.lines:
        r = line.r
        t = math.degrees(line.t)
        # print('t',t, 'r',r)
        length = getLineLength(line)
        if (math.fabs(t - constants.EXPECTED_FRONT_LINE_T) < 30.0\
        or math.fabs(t - constants.EXPECTED_FRONT_LINE_T_2) < 30.0) \
        and math.fabs(r - constants.EXPECTED_FRONT_LINE_R) < 15.0\
        and r != 0.0:
            # print("found front line!", "r", r, "t", t)
            return False

    # print "NO top line!!!"
    return True

def shouldBackUp(player):
    # If we cannot see any good lines, should probably back up!

    for line in GoalieStates.watchWithLineChecks.lines:
        r = line.r
        t = math.degrees(line.t)
        # print("found line!", r)

        if r < 130.0 and r != 0.0:
            return False

    for i in range(0, player.brain.visionLines.line_size()):
        r = player.brain.visionLines.line(i).inner.r
        if r < 130.0 and r != 0.0:
            return False


    if player.brain.visionLines.line_size() == 0:
        print "My brain sees no lines right now"
        print len(GoalieStates.watchWithLineChecks.lines)
        return True

    print "Couldn't find any good lines, backup TRUE"
    return True

def shouldStopGoingBack(player):
    visionLines = player.brain.visionLines
    # Add in checks to not add repeat lines?

    for i in range(0, visionLines.line_size()):
        r = visionLines.line(i).inner.r
        t = math.degrees(visionLines.line(i).inner.t)

        if r != 0.0 and r < 40.0 and r > 25.0 and \
        (math.fabs(t - constants.EXPECTED_FRONT_LINE_T) < 50.0\
        or math.fabs(t - constants.EXPECTED_FRONT_LINE_T_2) < 50.0):
            print "I see a line now! I should probably stop going backwards"
            print ("r: ", r)
            print ("t: ", t)
            return True

    return False

def getBearingFromRobot(x, y):
    return math.degrees(math.atan2(x, -y));

def facingASideline(player):
    visionLines = player.brain.visionLines
    # Add in checks to not add repeat lines?

    if GoalieStates.watchWithLineChecks.numFixes > 2:
        return False

    for i in range(0, visionLines.line_size()):
        r1 = visionLines.line(i).inner.r
        t1 = math.degrees(visionLines.line(i).inner.t)
        length1 = getLineLength(visionLines.line(i).inner)
        # ep01 = visionLines.line(i).inner.ep0
        # ep11 = visionLines.line(i).inner.ep1

        for j in range(0, visionLines.line_size()):
            r2 = visionLines.line(j).inner.r
            t2 = math.degrees(visionLines.line(j).inner.t)
            length2 = getLineLength(visionLines.line(j).inner)
            # ep02 = visionLines.line(j).inner.ep0
            # ep12 = visionLines.line(j).inner.ep1

            if ((math.fabs(math.fabs(t1 - t2) - 90.0) < 15.0) \
            or (math.fabs(math.fabs(t1 - t2) - 265) < 15.0)) \
            and r1 != 0.0 and r2 != 0.0 and r1 < 170.0 and r2 < 170.0\
            and i is not j:
                print ("MY loc h: ", player.brain.loc.h)
                if player.brain.loc.h > 0.0:
                    print "I THINK I SEE MY RIGHT CORNER"
                else:
                    print "I THINK I SEE MY LEFT COrner"
                print "-------------------------"
                print ("R", r1)
                print ("R2", r2)
                print ("T", t1)
                print ("T2", t2)
                print ("length1:", length1, "length2:", length2)
                print ("horizon dist:", player.brain.vision.horizon_dist)
                print ("num corners:", player.brain.visionCorners.corner_size())

                for k in range(0, player.brain.visionCorners.corner_size()):
                    c = player.brain.visionCorners
                    print("x", c.corner(k).x, "y", c.corner(k).y)

                facefrnt = (player.brain.vision.horizon_dist > 600) and \
                math.fabs(math.degrees(player.brain.interface.joints.head_yaw)) < 10.0
                print("Facing front: ", facefrnt)

                if GoalieStates.watchWithLineChecks.wentToClearIt and\
                not GoalieStates.watchWithLineChecks.correctFacing:
                    if VisualGoalieStates.clearIt.ballSide == constants.RIGHT:
                        player.homeDirections += [RelRobotLocation(0,0,50.0)]
                        print "I think I'm facing right, so I'm turning left!"
                    else:
                        player.homeDirections += [RelRobotLocation(0,0,-50.0)]
                        print "I think I'm facing left, so I'm turning right!"
                    return True

                elif (GoalieStates.watchWithLineChecks.correctFacing and r1 > 20.0\
                                and r2 > 20.0) or facefrnt:
                    if math.fabs(math.fabs(t1 - 180.0) - 180.0) < math.fabs(math.fabs(t2 - 180.0) - 180.0):
                        frontline = visionLines.line(i).inner
                        sideline = visionLines.line(j).inner
                    else:
                        frontline = visionLines.line(j).inner
                        sideline = visionLines.line(i).inner

                    flength = getLineLength(frontline)
                    slength = getLineLength(sideline)
                    print("front line length:", flength, "sideline length:", slength)

                    #TODO adjust this to new line coords
                    # If sideline bearing is to my right, assume right sideline
                    if math.degrees(sideline.t) > 180.0:
                        print "I think this is my RIGHT sideline, I'm moving away"
                        y_dest = constants.EXPECTED_SIDE_LINE_R - sideline.r
                    else:
                        print "I think this is my LEFT sideline, I'm moving away"
                        y_dest = sideline.r - constants.EXPECTED_SIDE_LINE_R

                    h_dest = 0 #frontline.t - 90.0
                    if math.fabs(y_dest) < 10.0:
                        print("adjustment too small, not doing it", y_dest)
                        return False
                    player.homeDirections += [RelRobotLocation(0.0, y_dest, h_dest)]
                    print("Sideline.r: ", sideline.r, "sideline.t:", math.degrees(sideline.t))
                    print ("I'm adusting myself by: y:", y_dest, "h:", h_dest)
                    return True

                # if GoalieStates.watchWithLineChecks.correctFacing:
                #     # Adjust heading appropriately
                #     print "I think my heading's right"
                # else:
                #     player.homeDirections += [RelRobotLocation(0, 0, -90)]

    return False

def getLineLength(line):
    return line.ep1 - line.ep0

def shouldPositionRight(player):
    if player.brain.ball.bearing_deg < -40.0 and \
    player.brain.ball.distance > constants.CLEARIT_DIST and \
    player.inPosition is not constants.RIGHT_POSITION and \
    player.inPosition is not constants.NOT_IN_POSITION:
        player.homeDirections = []
        player.homeDirections += [RelRobotLocation(5.0, -30.0, -15.0)]
        return True

    GoalieStates.watchWithLineChecks.shiftedPosition = False
    return False

def shouldPositionLeft(player):
    if player.brain.ball.bearing_deg > 40.0 and \
    player.brain.ball.distance > constants.CLEARIT_DIST and \
    player.inPosition is not constants.LEFT_POSITION and \
    player.inPosition is not constants.NOT_IN_POSITION:
        player.homeDirections = []
        player.homeDirections += [RelRobotLocation(5.0, 30.0, 15.0)]
        return True

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

def seeFrontLine(player):
    lines = player.brain.visionLines
    horizon = player.brain.vision.horizon_dist

    for i in range(0, lines.line_size()):
        r = lines.line(i).inner.r
        t = math.degrees(lines.line(i).inner.t)
        length = getLineLength(lines.line(i).inner)
        print ("R:", r, "T:", t)

def lostBall(player):
    if player.brain.ball.vis.frames_off > 15:
        return True
    return False

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
    # return (math.fabs(player.brain.ball.bearing_deg) < 10.0 and
    return ((player.brain.nav.currentState == 'standing' and \
                    player.brain.ball.vis.on) \
            or (math.fabs(player.brain.ball.bearing_deg) < 15.0))

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

    nball = player.brain.naiveBall

    # if (nball.x_vel < -10.0 and
    if (ball.mov_vel_x < -10.0 and
        not nball.stationary and
        nball.yintercept < -20.0 and
        ball.distance < 140.0 and
        sightOk):
        print "DIVE RIGHT"
        print("Ball dist:", ball.distance)
        print("shouldDiveRight.lastFramesOff:", shouldDiveRight.lastFramesOff)
        print("ball.vis.frames_on", ball.vis.frames_on)
        print("nb xvel:", nball.x_vel)
        print("ball mov vel:", ball.mov_vel_x)

    # return (nball.x_vel < -10.0 and
    return (ball.mov_vel_x < constants.SAVE_X_VEL and
        not nball.stationary and
        nball.yintercept < -25.0 and
        ball.distance < constants.SAVE_DIST and
        sightOk)

    # return (ball.mov_vel_x < -6.0 and
    #         ball.mov_speed > 8.0 and
    #         ball.rel_y_intersect_dest < -20.0 and
    #         ball.distance < 150.0 and
    #         sightOk)

def shouldDiveLeft(player):

    if player.firstFrame():
        shouldDiveLeft.lastFramesOff = 21

    sightOk = True
    ball = player.brain.ball

    if shouldDiveLeft.lastFramesOff > 20 and ball.vis.frames_on < 20:
        sightOk = False

    if not ball.vis.on:
        shouldDiveLeft.lastFramesOff = ball.vis.frames_off

    nball = player.brain.naiveBall

    # if (nball.x_vel < -10.0 and
    if (ball.mov_vel_x < constants.SAVE_X_VEL and
        not nball.stationary and
        nball.yintercept > 20.0 and
        ball.distance < 140.0 and
        sightOk):
        print "DIVE LEFT"
        print("Ball dist:", ball.distance)
        print("shouldDiveRight.lastFramesOff:", shouldDiveRight.lastFramesOff)
        print("ball.vis.frames_on", ball.vis.frames_on)
        print("nb xvel:", nball.x_vel)
        print("ball mov vel:", ball.mov_vel_x)

    # return (nball.x_vel < -10.0 and
    return (ball.mov_vel_x < -9.0 and
       not nball.stationary and
        nball.yintercept > 25.0 and
        ball.distance < constants.SAVE_DIST and
        sightOk)

    # return (ball.mov_vel_x < -6.0 and
    #         ball.mov_speed > 8.0 and
    #         ball.rel_y_intersect_dest > 20.0 and
    #         ball.distance < 150.0 and
    #         sightOk)

def shouldSquat(player):

    if player.firstFrame():
        shouldSquat.lastFramesOff = 21

    sightOk = True
    ball = player.brain.ball

    if shouldSquat.lastFramesOff > 20 and ball.vis.frames_on < 20:
        sightOk = False

    if not ball.vis.on:
        shouldSquat.lastFramesOff = ball.vis.frames_off

    nball = player.brain.naiveBall

    # if (nball.x_vel < -10.0 and
    if (ball.mov_vel_x < constants.SAVE_X_VEL and
        not nball.stationary and
        abs(nball.yintercept) < 25.0 and
        nball.yintercept != 0.0 and
        ball.distance < constants.SAVE_DIST and
        sightOk):
        print "SQUAT"
        print("Ball dist:", ball.distance)
        print("shouldDiveRight.lastFramesOff:", shouldDiveRight.lastFramesOff)
        print("ball.vis.frames_on", ball.vis.frames_on)
        print("nb xvel:", nball.x_vel)
        print("ball mov vel:", ball.mov_vel_x)

    # TODO Lower threshold for fast balls
    # if nball.x_vel < -30.0 and abs(nball.yintercept)

    # More sensitive to close balls even at lower speeds
    if ball.distance < 50.0 and ball.mov_vel_x < -6.0 and nball.yintercept != 0.0\
    and abs(nball.yintercept) < 30.0:
        print("ball exceptionally close, I'm saving")
        return True

    if ball.distance < 40.0 and ball.mov_vel_x < -2.0:
        print("ball exceptionally close, I'm saving")
        return True

    # return (nball.x_vel < -10.0 and
    return (ball.mov_vel_x < -9.0 and
        not nball.stationary and
        abs(nball.yintercept) < 30.0 and
        nball.yintercept != 0.0 and
        ball.distance < 150.0 and
        sightOk)

    # return (ball.mov_vel_x < -4.0 and
    #         ball.mov_speed > 8.0 and
    #         abs(ball.rel_y_intersect_dest) < 40.0 and
    #         ball.distance < 150.0 and
    #         sightOk)

def shouldClearDangerousBall(player):
    return False
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
    if (player.brain.ball.distance < constants.CLEARIT_DIST):
        walkedTooFar.xThresh = 120.0
        walkedTooFar.yThresh = 120.0
        shouldGo = True

    # farther out but being aggressive
    # if (player.brain.ball.distance < 130.0 and
    #     player.brain.ball.is_stationary and
    #     player.aggressive):
    #     walkedTooFar.xThresh = 150.0
    #     walkedTooFar.yThresh = 150.0
    #     shouldGo = True

    # to goalie's sides, being aggressive
    # if (math.fabs(player.brain.ball.bearing_deg) > 50.0 and
    #     player.aggressive):
    #     walkedTooFar.xThresh = 300.0
    #     walkedTooFar.yThresh = 300.0
    #     shouldGo = True

    if shouldGo:
        if player.brain.ball.bearing_deg < -65.0:
            VisualGoalieStates.clearIt.dangerousSide = constants.RIGHT
        elif player.brain.ball.bearing_deg > 65.0:
            VisualGoalieStates.clearIt.dangerousSide = constants.LEFT
        else:
            VisualGoalieStates.clearIt.dangerousSide = -1

        if player.brain.ball.bearing_deg < 0.0:
            VisualGoalieStates.clearIt.ballSide = constants.RIGHT
        else:
            VisualGoalieStates.clearIt.ballSide = constants.LEFT

        print ("Ball dist:", player.brain.ball.distance)

    return shouldGo

def ballLostStopChasing(player):
    """
    If the robot does not see the ball while chasing, it is lost. Delay
    in case our shoulder pads are just hiding it.
    """
    if player.brain.ball.vis.frames_off > 30:
        return True

#TODO fix this and make more sensitive
def ballMovedStopChasing(player):
    """
    If the robot has been chasing for a while and it is far away, it should
    stop chasing.
    """
    return (player.brain.ball.distance > 50.0 and
            player.counter > 175.0)

def walkedTooFar(player):
    # for the odometry reset delay
    if player.counter < 3:
        return False

    if player.aggressive:
        return False

    # if player

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
    return player.counter > 50 #and VisualGoalieStates.clearIt.dangerousSide == -1

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
