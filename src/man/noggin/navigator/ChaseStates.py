from . import NavHelper as helper
from . import WalkHelper as walker
from ..navigator import BrunswickSpeeds as speeds
from math import (sin, cos)
from ..util import MyMath
DEBUG = False

def walkSpinToBall(nav):

    ball = nav.brain.ball
    nav.dest = ball
    nav.dest.h = ball.heading

    # Set our walk towards the ball
    walkX, walkY, walkTheta = \
           walker.getWalkSpinParam(nav.brain.my, nav.dest)

    helper.setSpeed(nav, walkX, walkY, walkTheta)

    #if we're close to the ball...
    if helper.atDestinationCloser(nav.brain.my, nav.dest):
        # and facing  it, stop
        if abs(ball.bearing) < 10:
            return nav.goNow('stop')

    return nav.stay()

# Values for controlling the strafing
PFK_MAX_Y_SPEED = speeds.MAX_Y_SPEED
PFK_MIN_Y_SPEED = speeds.MIN_Y_SPEED
PFK_MAX_X_SPEED = speeds.MAX_X_SPEED
PFK_MIN_X_SPEED = speeds.MIN_X_MAGNITUDE
PFK_MIN_Y_MAGNITUDE = speeds.MIN_Y_MAGNITUDE
PFK_X_GAIN = 0.12
PFK_Y_GAIN = 0.6


def positionForKick(nav):
    ## nav.dest = kick.getKickPosition()

    ## sX,sY,sTheta = walker.getOmniWalkParam(nav.brain.my, nav.dest)

    ball = nav.brain.ball

    # Determine approach speed
    relY = sin(ball.bearing) * ball.dist
    relX = cos(ball.bearing) * ball.dist

    sY = MyMath.clip(relY * PFK_Y_GAIN,
                     PFK_MIN_Y_SPEED,
                     PFK_MAX_Y_SPEED)

    sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

    if ball.dist > 5:
        sX = MyMath.clip(relX * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
    else:
        sX = 0.0

    helper.setSpeed(nav,sX,sY,0)
    return nav.stay()
