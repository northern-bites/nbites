from man.noggin.util import MyMath
from . import WalkHelper as walker
from . import NavHelper as helper
from . import NavConstants as constants
from . import BrunswickSpeeds as speeds
from math import fabs

# Values for controlling the speeds
PFK_LEFT_SPIN_SPEED = speeds.LEFT_SPIN_WHILE_X_MAX_SPEED
PFK_RIGHT_SPIN_SPEED = speeds.RIGHT_SPIN_WHILE_X_MAX_SPEED
PFK_LEFT_SPEED = speeds.OMNI_LEFT_MAX_SPEED
PFK_RIGHT_SPEED = speeds.OMNI_RIGHT_MAX_SPEED
PFK_FWD_SPEED = speeds.OMNI_FWD_MAX_SPEED
PFK_REV_SPEED = speeds.OMNI_REV_MAX_SPEED
PFK_MIN_Y_MAGNITUDE = speeds.MIN_OMNI_Y_MAGNITUDE
PFK_MIN_X_MAGNITUDE = speeds.MIN_OMNI_X_MAGNITUDE
PFK_MIN_SPIN_MAGNITUDE = speeds.MIN_SPIN_WHILE_X_MAGNITUDE

# Buffering values, insure that we eventually kick the ball
PFK_CLOSE_ENOUGH_X = 2.7
PFK_CLOSE_ENOUGH_Y = 2.7
PFK_CLOSE_ENOUGH_THETA = 11
PFK_MAX_X_SPEED_DIST = 80
PFK_MAX_Y_SPEED_DIST = 20

"""
Control State for PFK. Breaks up the problem into
Theta, Y, and X. We want to make sure that we kick
and don't try to get perfect position, so we get
each direction good enough and then don't look at
it again.
"""
def pfk_all(nav):

    sX = 0.0            # speed in the x direction
    sY = 0.0            # speed in the y direction
    sTheta = 0.0        # speed in the theta direction

    if nav.firstFrame():
        nav.stopTheta = True
        nav.stopY = False
        nav.stopX = False
        #print "entered from: ", nav.lastDiffState

    # get our ideal relative positionings from the kick
    (x_offset, y_offset, heading) = nav.kick.getPosition()

    ball = nav.brain.ball

    """
    # determine the theta speed if our position isn't good enough
    if not nav.stopTheta:
        sTheta = pfk_theta(nav, ball, heading)
    """

    # determine the y speed if our position isn't good enough
    if not nav.stopY:
        sY = pfk_y(nav, ball, y_offset)
        #if sY interferes with what sTheta says, dangerous ball.
            #Move X and Y only?

    # determine the x speed if our position isn't good enough
    if not nav.stopX:
        sX = pfk_x(nav, ball, x_offset)
        if (sX < 0):
            #print "dangerous ball detected during PFK"
            helper.setSpeed(nav, sX, 0, 0)
            return nav.stay()

    if (nav.stopX and nav.stopY and nav.stopTheta):
        # in good position for kick
        return nav.goNow('stop')

    helper.setSpeed(nav, sX, sY, sTheta)

    return nav.stay()

"""
Determines the speed in the theta direction to position
accurately on the ball and returns that value
"""
#def pfk_theta(nav, ball, targetTheta)
#
# NOT IMPLEMENTED!!! Curently all theta positioning is done by approachBall.
#
# Must use a global heading target based on a global robot heading.
# Currently loc is unreliable so this cannot be done well. Ideally,
# the kick would be determined at the time we decide to chase the ball,
# changed dynamically if we start recognizing other robots, or if the
# ball moves along the way. This would mean positioning along a good
# theta direction from the start to reach the ball effectively and then
# fine turning for the kick. Once walking to a destination is implemented
# this will be much more achievable. Work around for now.

"""
Determines the speed in the y direction to position
accurately on the ball and returns that value
"""
def pfk_y(nav, ball, targetY):

    targetDist = ball.relY - targetY

    if (fabs(targetDist) <= PFK_CLOSE_ENOUGH_Y):
        nav.stopY = True
        return 0

    if (targetDist > 0):
        # Move left to match ball with target
        # Change the distance to a speed and clip within vector limits
        sY = MyMath.clip(targetDist/PFK_MAX_Y_SPEED_DIST,
                           PFK_MIN_Y_MAGNITUDE,
                           PFK_LEFT_SPEED)
        return sY

    else:
        # Move right to match ball with target
        # Change the distance to a speed and clip within vector limits
        sY = MyMath.clip(targetDist/PFK_MAX_Y_SPEED_DIST,
                           PFK_RIGHT_SPEED,
                           -PFK_MIN_Y_MAGNITUDE)

        return sY
"""
Determines the speed in the x direction to position
accurately on the ball and returns that value
"""
def pfk_x(nav, ball, targetX):

    targetDist = ball.relX - targetX

    if (fabs(targetDist) <= PFK_CLOSE_ENOUGH_X):
        nav.stopX = True
        return 0

    if (targetDist > 0):
        # Move foward to match ball with target
        # Change the distance to a speed and clip within vector limits
        sX = MyMath.clip(targetDist/PFK_MAX_X_SPEED_DIST,
                           PFK_MIN_X_MAGNITUDE,
                           PFK_FWD_SPEED)

        return sX

    else:
        # Move Backwards Slowly
        return -.5
