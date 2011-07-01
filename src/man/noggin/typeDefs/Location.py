from math import (degrees,
                  hypot,
                  atan2,
                  fabs)

from ..util import MyMath
from .. import NogginConstants
from . import LocationConstants as constants

class Location (object):

    def __init__(self, x = 0, y = 0, z = 0):
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

    def __str__(self):
        return ("x: %g  y: %g z: %g" % (self.x, self.y, self.z))

    def toTupleXY(self):
        return (self.x, self.y)

    def distTo(self, other, forceCalc = False):
        """
        returns euclidian dist
        """

        # if we're calculating distance from us to the ball use stored value
        if not forceCalc and hasattr(self, "teamColor") and \
               hasattr(other, "dist"):
            return other.dist

        # HACK HACK HACK HACK for infinity values HACK HACK
        if other.x == float('inf') or \
               other.y == float('inf'):
            print "WE HAVE AN INFINITY = ", self.x, self.y, other.x, other.y
            return 10000.
        return hypot(other.y - self.y, other.x - self.x)

    def headingTo(self, other, forceCalc = False):
        '''determine the heading facing a target x, y'''

        if not forceCalc and hasattr(self, "teamColor") and \
               hasattr(other, "heading"):
            return other.heading

        return degrees(atan2(other.y - self.y, other.x - self.x))

    def inOppGoalBox(self):

        return ((NogginConstants.OPP_GOALBOX_LEFT_X -
                 NogginConstants.BOX_BUFFER) < self.x and
                self.x < (NogginConstants.OPP_GOALBOX_RIGHT_X +
                          NogginConstants.BOX_BUFFER) and
                (NogginConstants.OPP_GOALBOX_TOP_Y +
                 NogginConstants.BOX_BUFFER) > self.y and
                self.y > (NogginConstants.OPP_GOALBOX_BOTTOM_Y -
                          NogginConstants.BOX_BUFFER))

    def inMyGoalBox(self):

        return (self.x < (NogginConstants.MY_GOALBOX_RIGHT_X +
                          NogginConstants.BOX_BUFFER) and
                (NogginConstants.MY_GOALBOX_TOP_Y +
                 NogginConstants.BOX_BUFFER) > self.y and
                self.y > (NogginConstants.MY_GOALBOX_BOTTOM_Y -
                          NogginConstants.BOX_BUFFER))

    def inCenterOfField(self):
        return NogginConstants.FIELD_HEIGHT *2./3. > self.y > \
               NogginConstants.FIELD_HEIGHT / 3.

    def inTopOfField(self):
        return NogginConstants.FIELD_HEIGHT*2./3. < self.y

    def inBottomOfField(self):
        return NogginConstants.FIELD_HEIGHT/3. > self.y

class RobotLocation(Location):

    def __init__(self, xP = 0, yP = 0, h = 0):
        Location.__init__(self, xP, yP)
        self.h = h

    def __str__(self):
        return (Location.__str__(self) +  " h: %g" % (self.h))

    def getRelativeBearing(self, other, forceCalc = False):
        """return relative heading in degrees from robot localization to
        abs x,y on field """

        # if we're calculating bearing from us(has a team color) to the ball use stored value
        if not forceCalc and hasattr(self, "teamColor") and \
               hasattr(other, "bearing"):
            return other.bearing

        return MyMath.sub180Angle(self.headingTo(other) - self.h)

    def spinDirToPoint(self, other):
        """
        Simple function to get the spin direction for a given point.
        """
        targetH = self.getRelativeBearing(other)

        if targetH == 0:
            return -MyMath.sign(self.h) # return 0???
        else:
            return MyMath.sign(targetH)

    def isFacingSideline(self):

        return (self.inTopOfField() and
                constants.FACING_SIDELINE_ANGLE > self.h >
                180.0 - constants.FACING_SIDELINE_ANGLE ) or \
                (self.inBottomOfField() and
                 -constants.FACING_SIDELINE_ANGLE > self.h >
                 -(180 - constants.FACING_SIDELINE_ANGLE) )

class RelLocation(RobotLocation):
    def __init__(self, my, dx, dy, dh):
        RobotLocation.__init__(self, my.x + dx, my.y + dy, my.h + dh)
        self.relX = dx
        self.relY = dy
        self.relH = dh

    def __str__(self):
        return "relX: %g, relY: %g, relH: %g"%(self.relX, self.relY, self.relH)
