from . import NavConstants as constants
from . import NavHelper as helper
from ..players import BrunswickSpeeds as speeds
from ..util import MyMath
from math import fabs
DEBUG = False

# we may want to allow this only when walk straight won't be able to adjust
# or we maybe in position except we're not facing the ball
def spinToBall(nav):

    if nav.firstFrame():
        nav.brain.CoA.setRobotGait(nav.brain.motion)

    ball = nav.brain.ball
    my = nav.brain.my

    nav.dest = ball
    nav.dest.h = my.getTargetHeading(ball)

    theta = MyMath.sign(ball.bearing) * constants.GOTO_SPIN_SPEED * \
             helper.getRotScale(ball.bearing)
    helper.setSpeed(nav, 0, 0, theta)

    # if we're facing the ball...
    if abs(ball.bearing) < 10:

        # and close to it, stop
        if helper.atDestinationCloser(nav.brain.my, nav.dest):
            return nav.goNow('stop')

        # but not close, walk towards it
        else:
            return nav.goLater('walkStraightToBall')

    return nav.stay()

def walkStraightToBall(nav):

    ball = nav.brain.ball
    nav.dest = ball
    nav.dest.h = nav.brain.my.getTargetHeading(ball)

    if ball.dist < constants.APPROACH_WITH_GAIN_DIST:
        sX = MyMath.clip(ball.dist*constants.APPROACH_X_GAIN,
                         constants.MIN_APPROACH_X_SPEED,
                         constants.MAX_APPROACH_X_SPEED)
    else :
        sX = constants.MAX_APPROACH_X_SPEED

    # Determine the speed to turn to the ball
    sTheta = MyMath.clip(ball.bearing*constants.APPROACH_SPIN_GAIN,
                         -constants.APPROACH_SPIN_SPEED,
                         constants.APPROACH_SPIN_SPEED)

    # is this necessary? as long as we have x of MIN_APPROACH_SPEED
    # it seems like we won't just step in place. if we are spinning only
    # we should be in a different state...
    # Avoid spinning so slowly that we step in place
    if fabs(sTheta) < constants.MIN_APPROACH_SPIN_MAGNITUDE:
        sTheta = 0.0

    # Set our walk towards the ball
    helper.setSpeed(nav, sX, 0, sTheta)

    #if we're close to the ball...
    if helper.atDestinationCloser(nav.brain.my, nav.dest):
        # and facing  it, stop
        if abs(ball.bearing) < 10:
            return nav.goNow('stop')
        # not facing it, spin to it
        else:
            return nav.goLater('spinToBall')

    # if we really need to, only spin to the ball
    if abs(ball.bearing) > 100:
        return nav.goLater('spinToBall')

    # if we need to adjust our heading a lot
    # may also try increasing APPROACH_SPIN_SPEED
    # we want to avoid stopping unless absolutely necessary
    if abs(ball.bearing) > 20:
        return nav.goLater('omniWalkToBall')

    return nav.stay()

def omniWalkToBall(nav):
    ball = nav.brain.ball
    nav.dest = ball
    # be nice if we could use bearing here instead...
    nav.dest.h = nav.brain.my.getTargetHeading(ball)

    walkX, walkY, walkTheta = \
           helper.getOmniWalkFacingDestParam(nav.brain.my, nav.dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    #if we're close to the ball...
    if helper.atDestinationCloser(nav.brain.my, nav.dest):
        # and facing  it, stop
        if abs(ball.bearing) < 10:
            return nav.goNow('stop')
        # not facing it, spin to it
        else:
            return nav.goLater('spinToBall')
        # if we really need to, only spin to the ball
    if abs(ball.bearing) > 100:
        return nav.goLater('spinToBall')

    if abs(ball.bearing) < 20:
        return nav.goLater('walkStraightToBall')

    return nav.stay()
