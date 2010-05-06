from . import NavConstants as constants
from . import NavHelper as helper
from ..players import BrunswickSpeeds as speeds
from ..util import MyMath
from math import fabs
DEBUG = False

def walkSpinToBall(nav):

    ball = nav.brain.ball
    nav.dest = ball
    nav.dest.h = ball.heading

    # Set our walk towards the ball
    walkX, walkY, walkTheta = \
           helper.getWalkSpinParam(nav.brain.my, nav.dest)

    helper.setSpeed(nav, walkX, walkY, walkTheta)

    #if we're close to the ball...
    if helper.atDestinationCloser(nav.brain.my, nav.dest):
        # and facing  it, stop
        if abs(ball.bearing) < 10:
            return nav.goNow('stop')

    return nav.stay()
