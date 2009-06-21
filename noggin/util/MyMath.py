
"""
myMath.py - a number of methods for simple often used math
"""

from math import (atan2,
                  cos,
                  degrees,
                  fabs,
                  hypot,
                  pi,
                  radians,
                  sin)


def sub180Angle(angle):
    """
    Returns the angle identitical to the input angle that is between -180 and
    180 degrees.
    """
    angle = angle % 360
    if angle > 180:
        angle -= 360.
    if angle < -180:
        angle += 360.
    return angle


def sub180Diff(angle, from_angle):
    """
    Returns the angle indetical to the first angle such that it has a
    Euclidian distance from from_angle of less that 180 degrees.
    """
    while (fabs(angle - from_angle) > 180.):
        if angle > 0.:
            angle -= 360
        else:
            angle += 360
    return angle

def dist(x1,y1,x2,y2):
    ''' returns euclidian dist'''
    # HACK HACK HACK HACK for infinity values HACK HACK
    if x2 == float('inf') or \
            y2 == float('inf'):
        print "WE HAVE AN INFINITY = ", x1,y1,x2,y2
        return 10000
    return hypot(y2-y1,x2-x1)

def safe_atan2(y, x):
    if x == 0.0:
        if y > 0:
            return pi / 2.0
        else:
            return -pi / 2.0
    else:
        return atan2(y, x)

def sign(x):
    """
    return the sign of a number
    """
    if x == 0:
        return 0
    else:
        return x/abs(x)

def clip(value,min,max):
    '''takes a value, plus a min and a max,
    then returns a value within the max/min'''
    if value < min:
        return min
    elif value > max:
        return max
    else:
        return value

def getRelativeX(dist,bearing):
    '''returns relative x of dist,bearing. dist (cms), bearing in deg'''
    return fabs(dist)*cos(radians(bearing))

def getRelativeY(dist,bearing):
    '''returns relative y of dist,bearing. dist (cms), bearing in deg'''
    return fabs(dist)*sin(radians(bearing))

def getRelativeVelocityX(robotH, velX, velY):
    '''Returns the x velocity of the ball relative to the self heading'''
    return velX * cos(radians(-robotH)) - velY * sin(radians(robotH))

def getRelativeVelocityY(robotH, velX, velY):
    '''Returns the y velocity of the ball relative to the self heading'''
    return velX * sin(radians(robotH)) + velY * cos(radians(robotH))

def getRelativeBearing(robotX, robotY, robotH, x, y):
    '''return relative heading from self localization to abs x,y on field'''
    return sub180Angle((degrees(safe_atan2(y - robotY,
                                           x - robotX))) - robotH)

def getTargetHeading(my, targetX, targetY):
    '''determine the heading facing a target x, y'''
    return sub180Angle(degrees(safe_atan2(targetY - my.y, targetX - my.x)))

def getSpinDir(h, targetH):
    """
    Advanced function to get the spin direction for a given heading.
    """
    LEFT_SPIN = 1
    RIGHT_SPIN = -1
    spinDir = 0

    if abs(h - targetH) < 5:
        spinDir = 0
    elif targetH == 0:
        spinDir = -sign(h)
    elif targetH == (180 or -180):
        spinDir = sign(h)
    elif sign(targetH) == sign(h):
        spinDir = sign(targetH - h)
    elif h < 0:
        if (h + 180) >= targetH:
            spinDir = LEFT_SPIN
        else: # h+180 < targetH
            spinDir = RIGHT_SPIN
    else: # h>0
        if (h - 180) >=targetH:
            spinDir = LEFT_SPIN
        else:
            spinDir = RIGHT_SPIN
    return spinDir
