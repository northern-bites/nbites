from .. import NogginConstants as Constants
from .VisualObject import VisualObject
from .LocObject import LocObject
from ..util.MyMath import (getRelativeX,
                           getRelativeY,
                           sub180Angle)

LOST_OBJECT_FRAMES_THRESH = 7

class FieldObject(VisualObject, LocObject):
    """
    FieldObject is a class for field landmarks, storing visual information
    -landmarks:
    -yglp,ygrp,bglp,bgrp (yellow and blue goal left and right posts)
    -myGoalLeftPost,myGoalRightPost,oppGoalLeftPost,oppGoalRightPost

    contains:
    -centerX,centerY -- center (x,y) coordinates of object on image screen
    -width,height -- width and height of blob in pixels
    -angleX,angleY -- angles of center (x,y) to focal point in degrees
    -dist -- distance from center of the body to ball in cms
    -bearing -- angle in x-axis from center of body to field object
    -elevation -- angle in y-axis from center of body to field object
    -framesOn -- # of consecutive frames the obj has been recognized in vision
    -framesOff -- # of consecutive frames the obj has been not recognized
    -on -- simple bool if field object is on vision frame or not
    -x,y -- coordinates of the landmark on the field
    """
    def __init__(self, visionInfos, visionName):
        """initialization all values for FieldObject() class"""
        # Things to be filled out later
        VisualObject.__init__(self)
        LocObject.__init__(self)
        self.localId = 0 # name based around team color
        self.dist = 0
        self.bearing = 0
        self.elevation = 0

        # Setup the data from vision
        self.visionId = visionName+20# ** # for clarity compared to field corners
        self.updateVision(visionInfos)

    def associateWithRelativeLandmark(self, relativeLandmark):
        """
        Method to set the properties of the object relative to the team color
        """
        self.x = relativeLandmark[0]
        self.y = relativeLandmark[1]
        self.localId = relativeLandmark[2]

    def updateVision(self, visionInfos):
        """updates class variables with new vision information"""
        VisualObject.updateVision(self, visionInfos)
        self.certainty = visionInfos.certainty
        self.distCertainty = visionInfos.distCertainty

    def updateLoc(self, loc, my):
        """updates class variables with localization information"""
        # Don't need to update x, y here because they stay the same.
        self.locDist = my.distTo(self, forceCalc=True)
        self.locBearing = my.getRelativeBearing(self, forceCalc=True)

    def updateBestValues(self):
        if self.on:
            self.bearing = self.visBearing
            self.dist = self.visDist

        # ** #caused errors: when !self.on, self.bearing != self.locBearing
        #elif self.framesOff < LOST_OBJECT_FRAMES_THRESH:
        #    pass

        else:
            self.bearing = self.locBearing
            self.dist = self.locDist

        self.relX = getRelativeX(self.dist, self.bearing)
        self.relY = getRelativeY(self.dist, self.bearing)

    def __str__(self):
        """returns string with all class variables"""
        return ("%s, %s at (%d,%d)" %
                (Constants.visionObjectTuple[self.visionId],
                 Constants.landmarkTuple[self.localId],
                 self.visDist, self.visBearing))

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
