from . import NavConstants as constants
from math import fabs
from man.noggin.util import MyMath

def getOmniWalkParam(my, dest):
    # we use distance and bearing to get relX, relY which we already have
    # for the ball. be nice not to recalculate it.
    relX, relY, relH = 0, 0, 0
    if hasattr(dest, "relX") and \
            hasattr(dest, "relY") and \
            hasattr(dest, "relH"):
        relX = dest.relX
        relY = dest.relY
        relH = dest.relH
    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)
        relY = MyMath.getRelativeY(distToDest, bearingDeg)
        relH = MyMath.sub180Angle(dest.h - my.h)

    # calculate forward speed
    forwardGain = 1./constants.APPROACH_X_WITH_GAIN_DIST
    sX = relX * forwardGain
    if fabs(sX) < constants.OMNI_MIN_X_MAGNITUDE:
        sX = 0
    else:
        sX = MyMath.clip(sX,
                         constants.OMNI_REV_MAX_SPEED,
                         constants.OMNI_FWD_MAX_SPEED)

    # calculate sideways speed
    strafeGain = 1./constants.APPROACH_Y_WITH_GAIN_DIST
    sY = relY * strafeGain
    if fabs(sY) < constants.OMNI_MIN_Y_MAGNITUDE:
        sY = 0
    else:
        sY = MyMath.clip(sY,
                         constants.OMNI_RIGHT_MAX_SPEED,
                         constants.OMNI_LEFT_MAX_SPEED,)

    # calculate spin speed
    if (fabs(relH) < 5.0):
        sTheta = 0.0
    else:
        spinGain = 1./constants.APPROACH_THETA_WITH_GAIN_DIST
        sTheta = relH * spinGain
        sTheta = MyMath.clip(sTheta,
                             constants.OMNI_MAX_RIGHT_SPIN_SPEED,
                             constants.OMNI_MAX_LEFT_SPIN_SPEED)

    # refine x and y speeds
    if (fabs(relH) > 50.):
        sX = 0
        sY = 0

    elif (fabs(relH) > 25.):
        sY = 0
        sX = sX*.5

    return (sX, sY, sTheta)

def getWalkSpinParam(my, dest):
    relX, relH = 0, 0
    if hasattr(dest, "relX") and \
            hasattr(dest, "relH"):
        relX = dest.relX
        relH = dest.relH
    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)
        relH = MyMath.sub180Angle(dest.h - my.h)

   # calculate forward speed
    forwardGain = 1./constants.APPROACH_X_WITH_GAIN_DIST
    sX = relX * forwardGain
    if fabs(sX) < constants.OMNI_MIN_X_MAGNITUDE:
        sX = 0
    else:
        sX = MyMath.clip(sX,
                         constants.OMNI_REV_MAX_SPEED,
                         constants.OMNI_FWD_MAX_SPEED)

    # calculate spin speed
    if (fabs(relH) < 5.0):
        sTheta = 0.0
    else:
        spinGain = 1./constants.APPROACH_THETA_WITH_GAIN_DIST
        sTheta = relH * spinGain
        sTheta = MyMath.clip(sTheta,
                             constants.OMNI_MAX_RIGHT_SPIN_SPEED,
                             constants.OMNI_MAX_LEFT_SPIN_SPEED)
    # Correct sX
    if fabs(relH)  > 50.:
        sX = 0

    return (sX, 0, sTheta)

def getWalkStraightParam(my, dest):
    relX, relH = 0, 0
    if hasattr(dest, "relX") and \
            hasattr(dest, "relH"):
        relX = dest.relX
        relH = dest.relH
    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)
        relH = MyMath.sub180Angle(dest.h - my.h)

    # calculate spin speed
    if (fabs(relH) < 5.0):
        sTheta = 0.0
    else: #spin first
        spinGain = 1./constants.APPROACH_THETA_WITH_GAIN_DIST
        sTheta = relH * spinGain
        sTheta = MyMath.clip(sTheta,
                             constants.OMNI_MAX_RIGHT_SPIN_SPEED,
                             constants.OMNI_MAX_LEFT_SPIN_SPEED)
        return (0, 0, sTheta)

   # calculate forward speed if h is good.
    forwardGain = 1./constants.APPROACH_X_WITH_GAIN_DIST
    sX = relX * forwardGain
    if fabs(sX) < constants.OMNI_MIN_X_MAGNITUDE:
        sX = 0
    else:
        sX = MyMath.clip(sX,
                         constants.GOTO_BACKWARD_SPEED,
                         constants.GOTO_FORWARD_SPEED)

    return (sX, 0, 0)

def getWalkBackParam(my, dest):
    relX, relH = 0, 0
    if hasattr(dest, "relX") and \
            hasattr(dest, "relH"):
        relX = dest.relX
        relH = dest.relH
    else:
        bearingDeg = my.getRelativeBearing(dest)
        distToDest = my.distTo(dest)
        relX = MyMath.getRelativeX(distToDest, bearingDeg)
        relH = MyMath.sub180Angle(dest.h - my.h)

    if not fabs(relH) > 150 :
        spinGain = 1./constants.APPROACH_THETA_WITH_GAIN_DIST
        sTheta = (180-relH) * spinGain
        sTheta = MyMath.clip(sTheta,
                             constants.OMNI_MAX_RIGHT_SPIN_SPEED,
                             constants.OMNI_MAX_LEFT_SPIN_SPEED)
        return ( 0, 0, sTheta)

    forwardGain = 1./constants.APPROACH_X_WITH_GAIN_DIST
    sX = relX * forwardGain
    sX = MyMath.clip(sX,
                     constants.GOTO_BACKWARD_SPEED,
                     constants.GOTO_FORWARD_SPEED)

    return (sX, 0, 0)

def getSpinOnlyParam(my, dest):
    # Determine the speed to turn
    # see if getRotScale can go faster

    bearing = my.getRelativeBearing(dest)
    if (fabs(bearing) < 5.0):
        sTheta = 0.0
    else:
        spinGain = 1./constants.APPROACH_THETA_WITH_GAIN_DIST
        sTheta = bearing * spinGain
        sTheta = MyMath.clip(sTheta,
                             constants.GOTO_RIGHT_SPIN_SPEED,
                             constants.GOTO_LEFT_SPIN_SPEED)

    sX, sY = 0, 0
    return (sX, sY, sTheta)

def getRotScale(headingDiff):
    absHDiff = fabs(headingDiff)

    return absHDiff / 90.0

def getCloseRotScale(headingDiff):
    absHDiff = fabs(headingDiff)

    return absHDiff / 50.0
