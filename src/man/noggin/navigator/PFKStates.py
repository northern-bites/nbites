from man.noggin.util import MyMath
from . import WalkHelper as walker
from . import NavHelper as helper
from . import NavConstants as constants
from . import BrunswickSpeeds as speeds
from math import fabs

# arbitrary constant to reduce start/stop spinadjustments. even lower might be good
START_SPIN_BEARING = 20.

# near the limit of when we can stop seeing the ball with our head down
# alternatively, the angle from body center to outside corner of foot
# to reduce bumping into ball
# higher miht be good
STOP_SPIN_BEARING = 75.

# dist to end of feet plus extra buffer
# changed to be lower (weren't getting close enough to ball)
SAFE_BALL_REL_X = 12.

# dist to end of feet (make sure we don't bump ball when spinning)
SAFE_TO_SPIN_DIST = 15.

# make sure we don't bump into ball when strafing towards it
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

# Buffering values, insure that we eventually kick the ball
PFK_CLOSE_ENOUGH_XY = 2.0
PFK_CLOSE_ENOUGH_THETA = 11
PFK_BALL_CLOSE_ENOUGH = 30
# stop rotating after 4 consecutive frames with good hDiff
ROTATE_THRESHOLD = 3

def pfk_all(nav):
    """
    ball bearing is inside safe margin for spinning
    try to get almost all heading adjustment done here
    move x, y, and theta
    """

    if nav.firstFrame():
        nav.stopTheta = 0

    (x_offset, y_offset, heading) = nav.brain.kickDecider.currentKick.getPosition()

    ball = nav.brain.ball

    # calculate spin speed
    #hDiff = MyMath.sub180Angle(nav.brain.my.h - heading)
    hDiff = MyMath.sub180Angle(ball.bearing)

    if (fabs(hDiff) < 5.0):
        sTheta = 0.0
    else:
        sTheta = MyMath.sign(hDiff) * constants.GOTO_SPIN_SPEED * \
                 walker.getRotScale(hDiff)

        #sTheta = hDiff * walker.getRotScale(hDiff)
        sTheta = MyMath.clip(sTheta,
                             constants.OMNI_MIN_SPIN_SPEED,
                             constants.OMNI_MAX_SPIN_SPEED)

    if fabs(hDiff) < PFK_CLOSE_ENOUGH_THETA:
        nav.stopTheta += 1
        if nav.stopTheta > ROTATE_THRESHOLD:
            return nav.goNow('pfk_xy')
    else:
        nav.stopTheta = 0

    # if the ball is outside safe bearing and we're spinning away from it
    # or we're spinning towards it but we're likely to spin into it...
    # or we're close enough to the correct bearing
    if fabs(ball.bearing) > STOP_SPIN_BEARING and \
       ((MyMath.sign(ball.bearing) != MyMath.sign(hDiff))
            or ball.relX < SAFE_BALL_REL_X \
            or sTheta == 0.0):
            return nav.goNow('pfk_xy')

    target_y = ball.relY - y_offset

    # arbitrary
    if fabs(target_y) < PFK_CLOSE_ENOUGH_XY:
        sY = 0
    else:
        sY = MyMath.clip(target_y * PFK_Y_GAIN,
                         PFK_MIN_Y_SPEED,
                         PFK_MAX_Y_SPEED)
        sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

    if sY == 0.0 and sTheta == 0.0:
        return nav.goLater('pfk_final')

    x_diff = ball.relX - SAFE_BALL_REL_X
    # arbitrary
    if fabs(x_diff) < PFK_CLOSE_ENOUGH_XY:
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
        print "bearing to ball: %g" % ball.bearing
        #return nav.goNow('pfk_all')

    (x_offset, y_offset, heading) = nav.brain.kickDecider.currentKick.getPosition()
    target_y = ball.relY - y_offset

    # arbitrary
    if fabs(target_y) < PFK_CLOSE_ENOUGH_XY:
        return nav.goNow('pfk_final')
    else:
        sY = MyMath.clip(target_y * PFK_Y_GAIN,
                         PFK_MIN_Y_SPEED,
                         PFK_MAX_Y_SPEED)
        sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

    x_diff = ball.relX - SAFE_BALL_REL_X
    # arbitrary
    if fabs(x_diff) < PFK_CLOSE_ENOUGH_XY:
        sX = 0.0
    else:
        sX = MyMath.clip(x_diff * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
        sX = max(PFK_MIN_X_MAGNITUDE,sX) * MyMath.sign(sX)

    # in position, let's kick the ball!
    if (sX == 0.0 and sY == 0.0):
        return nav.goNow('stop')

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

    print "ball distance in final: ", ball.dist

    if ball.dist > PFK_BALL_CLOSE_ENOUGH:
        return nav.goNow('pfk_all')

    (x_offset, y_offset, heading) = nav.brain.kickDecider.currentKick.getPosition()

    x_diff = ball.relX - x_offset

    # arbitrary
    if fabs(x_diff) < PFK_CLOSE_ENOUGH_XY:
        sX = 0.0
    else:
        sX = MyMath.clip(x_diff * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
        sX = max(PFK_MIN_X_MAGNITUDE,sX) * MyMath.sign(sX)

    helper.setSpeed(nav,sX, 0.0, 0.0)

    # kicking time!
    if sX == 0.0:
        return nav.goNow('stop')

    return nav.stay()
