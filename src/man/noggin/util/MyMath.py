
"""
myMath.py - a number of methods for simple often used math
"""

from math import (atan2,
                  cos,
                  fabs,
                  radians,
                  degrees,
                  sin,
                  sqrt)


def sub180Angle(angle):
    """
    Returns the angle identitical to the input angle that is between -180 and
    180 degrees.
    """
    if angle > 180.:
        angle -= 360.
    if angle < -180.:
        angle += 360.
    return angle


def sub180Diff(angle, from_angle):
    """
    Returns the angle indetical to the first angle such that it has a
    Euclidian distance from from_angle of less that 180 degrees.
    """
    while (fabs(angle - from_angle) > 180.):
        if angle > 0.:
            angle -= 360.
        else:
            angle += 360.
    return angle

def sign(x):
    """
    return the sign of a number
    """
    if x == 0:
        return 0
    elif x > 0:
        return 1
    else:
        return -1

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

def distance3d(a, b):
    ''' Returns the euclidian distance between two 3d points'''
    return distanceNd(a, b)

def distanceNd(ptA, ptB):
    '''
    Returns the euclidian distance between any two n-dimensional pts
    If the points are of a different dimensionality, return the distance
    between the smaller point and the corresponding low dimensions of the larger
    '''
    # add up all the terms of form (ptA[i] - ptB[i])^2
    dimensionality = min(len(ptA), len(ptB))
    differences = 0

    for i in range(0, dimensionality):
        differences += pow((ptA[i] - ptB[i]), 2)

    return sqrt(differences)

def linesIntersect(x1,y1, x2, y2,
                     u1,v1, u2,v2):
    # Both lines are vertical, parallel and can't intersect
    if u1 == u2 and x1 == x2:
        return False

    if u1 == u2:
        y = ((y1-y2)/(x1-x2)) * (u1 - x1) + y1
        return v1 < y < v2 or v2 < y < v2

    if x1 == x2:
        y = ((v1-v2)/(u1-u2)) * (x1 - u1) + v1
        return y1 < y < y2 or y2 < y < y1

    b1 = (y2-y1)/(x2-x1)
    b2 = (v2-v1)/(u2-u1)

    if b1 == b2: return False

    a1 = y1-b1*x1
    a2 = v1-b2*u1

    xi = - (a1-a2)/(b1-b2)
    yi = a1+b1*xi

    return (x1-xi)*(xi-x2)>=0 and (u1-xi)*(xi-u2)>=0 and \
        (y1-yi)*(yi-y2)>=0 and (v1-yi)*(yi-v2)>=0
