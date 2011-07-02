from man.noggin.util import MyMath
from . import WalkHelper as walker
from . import NavHelper as helper
from . import NavConstants as constants
from . import BrunswickSpeeds as speeds
from math import fabs


def positionForKick(nav):
    """
    This state is called by player through Navigator::kickPosition(kick)
    It will position the robot at the ball using self.kick to determine the x,y
    offset and the final heading.

    This state will aggresively omni-walk, so it's probably best if we don't call
    it until we're near the ball.
    """
    ball = nav.brain.ball

    # we've either just started, or are close to our last destination
    # tell the robot where to go!
    if nav.firstFrame():
        nav.destX = ball.relX - nav.kick.x_offset -2 # HACK!!!
        nav.destY = ball.relY - nav.kick.y_offset

        nav.destTheta = ball.bearing

        # TODO later?
        #nav.destTheta = nav.kick.heading - nav.brain.my.h

        # slow down as we get near the ball (max 80% speed)
        if ball.dist < 30:
            nav.destGain = (0.4 + (ball.dist / 30)) * .8
        else:
            nav.destGain = .8

        nav.newDestination = True

        print 'Ball rel X: {0} Y: {1} ball bearing: {2}' \
              .format(ball.relX, ball.relY, ball.bearing)
        print 'Set new PFK destination of ({0}, {1}, {2}, gain={3})' \
              .format(nav.destX, nav.destY, nav.destTheta, nav.destGain)

        nav.brain.speech.say("New destination")

        return nav.goNow('destWalking')
