from .. import NogginConstants as Constants
from .VisualObject import VisualObject
from .LocObject import LocObject
from ..util.MyMath import (getRelativeX,
                           getRelativeY,
                           sub180Angle)

LOST_OBJECT_FRAMES_THRESH = 7

class FieldCorner(LocObject):
    """
    FieldCorner is a class for corners, storing localization information.
    """
    def __init__(self, Id):
        super(FieldCorner,self).__init__()
        self.visionId = Id
        self.elevation = 0
        self.framesOn = 0
        self.framesOff = 0
        self.height = 0
        self.dist = 0
        self.bearing = 0
        self.visDist = 0
        self.visBearing = 0
        self.x = 0
        self.y = 0
        self.angleX = 0
        self.angleY = 0
        self.on = False # Is the corner in vision this frame?
        self.onThisFrame = False

    def associateWithRelativeLandmark(self, relativeLandmark):
        """
        Method to set the properties of the object relative to the team color
        """
        self.x = relativeLandmark[0]
        self.y = relativeLandmark[1]

    def updateVision(self):
        if not self.onThisFrame:
            # Was not on frame; reset vision values
            self.framesOn = 0
            self.framesOff += 1
            self.angleX = 0
            self.angleY = 0
            self.visDist = 0
            self.visBearing = 0
            self.on = False

        self.onThisFrame = False

    def setVisualCorner(self, corner):
        # Found a visual corner we believe is this field corner.
        self.framesOn += 1
        self.framesOff = 0
        self.on = True
        self.onThisFrame = True

        # Update visual angle values.
        self.angleX = corner.angleX
        self.angleY = corner.angleY
        self.visDist = corner.dist
        self.visBearing = corner.bearing

    def updateLoc(self, loc, my):
        """
        Updates class variables with localization information.
        """
        self.locDist = my.distTo(self, forceCalc=True)
        self.locBearing = my.getRelativeBearing(self, forceCalc=True)

    def updateBestValues(self):
        if self.on > 0:
            self.bearing = self.visBearing
            self.dist = self.visDist
        else:
            self.bearing = self.locBearing
            self.dist = self.locDist

        self.relX = getRelativeX(self.locDist, self.locBearing)
        self.relY = getRelativeY(self.locDist, self.locBearing)
