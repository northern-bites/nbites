from math import (degrees,
                  hypot)

from ..util import MyMath

class Location (object):

    def __init__(self, x, y, z = 0):
        self.x = x
        self.y = y
        self.z = z

    def __eq__(self, other):
        return (self.x == other.x and
                self.y == other.y and
                self.z == other.z)

    def __ne__(self, other):
        return not (self.x == other.x and
                    self.y == other.y and
                    self.z == other.z)

    def dist(self, other):
        ''' returns euclidian dist'''
        # HACK HACK HACK HACK for infinity values HACK HACK
        if other.x == float('inf') or \
               other.y == float('inf'):
            print "WE HAVE AN INFINITY = ", self.x, self.y, other.x, other.y
            return 10000
        return hypot(other.y - self.y, other.x - self.x)

    def visible():
        pass
    def inScanRange():
        pass

class RobotLocation(Location):

    def __init__(self, xP, yP, h):
        Location.__init(x=xP, y=yP)
        self.h = h

    def getRelativeBearing(self, other):
        '''return relative heading from robot localization to abs x,y on field'''
        return MyMath.sub180Angle((degrees(MyMath.safe_atan2(other.y - self.y,
                                               other.x - self.x))) - self.h)

    def headingTo(self, other):
        '''determine the heading facing a target x, y'''
        return MyMath.sub180Angle(degrees(MyMath.safe_atan2(other.y - self.y,
                                              other.x - self.x)))

    def spinDirToPoint(self, other):
        """
        Advanced function to get the spin direction for a given point.
        """
        LEFT_SPIN = 1
        RIGHT_SPIN = -1
        spinDir = 0

        targetH = self.getRelativeBearing(other)

        if abs(self.h - targetH) < 5:
            spinDir = 0

        elif targetH == 0:
            spinDir = -MyMath.sign(self.h)

        elif targetH == (180 or -180):
            spinDir = MyMath.sign(self.h)

        elif MyMath.sign(targetH) == MyMath.sign(self.h):
            spinDir = MyMath.sign(targetH - self.h)

        elif self.h < 0:
            if (self.h + 180) >= targetH:
                spinDir = LEFT_SPIN
            else: # h+180 < targetH
                spinDir = RIGHT_SPIN

        else: # h>0
            if (self.h - 180) >= targetH:
                spinDir = LEFT_SPIN
            else:
                spinDir = RIGHT_SPIN

        return spinDir

    def spinDirToHeading(self, targetH):
        """
        Advanced function to get the spin direction for a given heading.
        """
        LEFT_SPIN = 1
        RIGHT_SPIN = -1
        spinDir = 0

        if abs(self.h - targetH) < 5:
            spinDir = 0
        elif targetH == 0:
            spinDir = -MyMath.sign(self.h)
        elif targetH == (180 or -180):
            spinDir = MyMath.sign(self.h)
        elif MyMath.sign(targetH) == MyMath.sign(self.h):
            spinDir = MyMath.sign(targetH - self.h)
        elif self.h < 0:
            if (self.h + 180) >= targetH:
                spinDir = LEFT_SPIN
            else: # h+180 < targetH
                spinDir = RIGHT_SPIN
        else: # h>0
            if (self.h - 180) >=targetH:
                spinDir = LEFT_SPIN
            else:
                spinDir = RIGHT_SPIN
        return spinDir

