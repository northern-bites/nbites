from man.noggin.util import MyMath
from . import WalkHelper as walker
from . import NavHelper as helper
from . import NavConstants as constants
from . import BrunswickSpeeds as speeds
from math import fabs

START_SPIN_BEARING = 40.
STOP_SPIN_BEARING = 75.
SAFE_BALL_REL_X = 18.
SAFE_TO_SPIN_DIST = 15.
SAFE_TO_STRAFE_DIST = 20.
# Values for controlling the strafing
PFK_MAX_Y_SPEED = speeds.MAX_Y_SPEED
PFK_MIN_Y_SPEED = speeds.MIN_Y_SPEED
PFK_MAX_X_SPEED = speeds.MAX_X_SPEED
PFK_MIN_X_SPEED = speeds.MIN_X_SPEED
PFK_MIN_Y_MAGNITUDE = speeds.MIN_Y_MAGNITUDE
PFK_MIN_X_MAGNITUDE = speeds.MIN_X_MAGNITUDE
PFK_X_GAIN = 0.12
PFK_Y_GAIN = 0.6

def pfk_all(nav):
    """
    ball bearing is inside safe margin for spinning
    try to get almost all heading adjustment done here
    move x, y, and theta
    """

    (x_offset, y_offset, heading) = nav.brain.kickDecider.currentKick.getPosition()

    ball = nav.brain.ball
    # calculate spin speed
    hDiff = MyMath.sub180Angle(nav.brain.my.h - heading)
    if (fabs(hDiff) < 5.0):
        sTheta = 0.0
    else:
        sTheta = MyMath.sign(hDiff) * constants.GOTO_SPIN_SPEED * \
                 walker.getRotScale(hDiff)

        #sTheta = hDiff * walker.getRotScale(hDiff)
        #sTheta = MyMath.clip(sTheta,
        #                     constants.OMNI_MIN_SPIN_SPEED,
        #                     constants.OMNI_MAX_SPIN_SPEED)

    # if the ball is outside safe bearing and we're spinning away from it
    # or we're spinning towards it but we're likely to spin into it...
    if fabs(ball.bearing) > STOP_SPIN_BEARING and \
       ((MyMath.sign(ball.bearing) != MyMath.sign(hDiff))
            or ball.relX < SAFE_BALL_REL_X):
            return nav.goNow('pfk_xy')

    target_y = ball.relY - y_offset

    if fabs(target_y) < 1.0:
        sY = 0
    else:
        sY = MyMath.clip(target_y * PFK_Y_GAIN,
                         PFK_MIN_Y_SPEED,
                         PFK_MAX_Y_SPEED)
        sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

    if sY == 0.0 and sTheta == 0.0:
        return nav.goLater('pfk_final')

    x_diff = ball.relX - SAFE_BALL_REL_X
    if fabs(x_diff) < 1.5:
        sX = 0.0
    else:
        sX = MyMath.clip(x_diff * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
        sX = max(PFK_MIN_X_MAGNITUDE,sX) * MyMath.sign(sX)

    print "hDiff:%g target_y:%g x_diff:%g" % (hDiff, target_y, x_diff)
    print "sTheta:%g sY:%g sX:%g" % (sTheta, sY, sX)
    helper.setSpeed(nav,sX,sY,sTheta)

    return nav.stay()

def pfk_xy(nav):
    """
    ball bearing is outside safe limit, we're in danger of losing the ball.
    position x,y only
    """

    ball = nav.brain.ball
    if ball.relX < SAFE_BALL_REL_X and \
           ball.dist < SAFE_TO_STRAFE_DIST:
        return nav.goNow('pfk_x')

    if fabs(ball.bearing) < START_SPIN_BEARING:
        return nav.goNow('pfk_all')

    (x_offset, y_offset, heading) = nav.brain.kickDecider.currentKick.getPosition()
    target_y = ball.relY - y_offset

    if fabs(target_y) < 1.0:
        sY = 0
    else:
        sY = MyMath.clip(target_y * PFK_Y_GAIN,
                         PFK_MIN_Y_SPEED,
                         PFK_MAX_Y_SPEED)
        sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

    x_diff = ball.relX - SAFE_BALL_REL_X
    if fabs(x_diff) < 1.5:
        sX = 0.0
    else:
        sX = MyMath.clip(x_diff * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
        sX = max(PFK_MIN_X_MAGNITUDE,sX) * MyMath.sign(sX)

    helper.setSpeed(nav,sX,sY,0.0)

    return nav.stay()

def pfk_x(nav):
    """
    ball is in a dangerous location next to our foot. move x only!
    """

    if nav.brain.ball.relX >= SAFE_BALL_REL_X or \
       nav.brain.ball.dist > SAFE_TO_STRAFE_DIST:
        return nav.goNow('pfk_xy')

    helper.setSpeed(nav, -PFK_MIN_X_SPEED, 0.0, 0.0)

    return nav.stay()

def pfk_final(nav):
    """
    we're done spinning and aligned for y.
    approach to final relX with x only.
    (may need to accept minor y changes in future)
    """

    ball = nav.brain.ball

    if ball.dist > 25.:
        return nav.goNow('pfk_all')

    (x_offset, y_offset, heading) = nav.brain.kickDecider.currentKick.getPosition()

    x_diff = ball.relX - x_offset

    if fabs(x_diff) < 1.:
        sX = 0.0
    else:
        sX = MyMath.clip(x_diff * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
        sX = max(PFK_MIN_X_MAGNITUDE,sX) * MyMath.sign(sX)

    helper.setSpeed(nav,sX, 0.0, 0.0)

    return nav.stay()
