from .. import NogginConstants as Constants
from .VisualObject import VisualObject

class FieldObject(VisualObject):
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
        self.x = 0
        self.y = 0
        self.localId = 0 # name based around team color
        self.locDist = 0
        self.locBearing = 0
        self.angleX = 0
        self.angleY = 0

        # Setup the data from vision
        self.visionId = visionName
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

        # set angleX, angleY so that we don't create c->python object overhead
        if self.dist > 0:
            self.angleX = (((Constants.IMAGE_WIDTH/2-1) - self.centerX)/
                           Constants.IMAGE_ANGLE_X)
            self.angleY = (((Constants.IMAGE_HEIGHT/2-1) - self.centerY)/
                           Constants.IMAGE_ANGLE_Y)
        else:
            self.angleX = 0
            self.angleY = 0

    def __str__(self):
        """returns string with all class variables"""
        return ("%s, %s at (%d,%d)s" %
                (Constants.visionObjectTuple[self.visionId],
                 Constants.landmarkTuple[self.localId])) + VisualObject.__str__(self)


class Line:
    """
    class for one single line, contains:
    -x1,y1 -- left coordinate on image screen
    -x2,y2 -- right coordinate on image screen
    -slope -- slope of line. remember that (0,0) is top-left corner of image
    -length -- length of the line
    """
    def __init__(self, visionLine):
        """init method"""
        self.x1 = visionLine.x1
        self.y1 = visionLine.y1
        self.x2 = visionLine.x2
        self.y2 = visionLine.y2
        self.slope = visionLine.slope
        self.length = visionLine.length

    def __str__(self):
        return ("p1: (%g,%g) p2: (%g,%g) slope: %g length: %g" %
                (self.x1,self.y1,self.x2,self.y1,self.slope,self.length))


class Crossbar(VisualObject):
    """
    Crossbar class, what used to be the backstop class
    """
    def __init__(self, visionInfos, visionName):
        """initialization of all values for Crossbar() class"""
        VisualObject.__init__(self)
        # Setup the data from vision
        self.visionId = visionName
        self.updateVision(visionInfos)

    def updateVision(self, visionInfos):
        """updates class variables with new vision information"""
        VisualObject.updateVision(self, visionInfos)
        self.x = visionInfos.x
        self.y = visionInfos.y
        self.angleX = visionInfos.angleX
        self.angleY = visionInfos.angleY
        self.elevation = visionInfos.elevation
        self.leftOpening = visionInfos.leftOpening
        self.rightOpening = visionInfos.rightOpening
        self.shoot = visionInfos.shoot
