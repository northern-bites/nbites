from .. import NogginConstants as Constants


class FieldObject:
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
        self.x = 0
        self.y = 0
        self.localId = 0 # name based around team color
        self.locDist = 0
        self.locBearing = 0
        self.angleX = 0
        self.angleY = 0
        self.framesOn = 0
        self.framesOff = 0
        self.on = False

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
        self.centerX = visionInfos.centerX
        self.centerY = visionInfos.centerY
        self.width = visionInfos.width
        self.height = visionInfos.height
        self.focDist = visionInfos.focDist
        self.dist = visionInfos.dist
        self.bearing = visionInfos.bearing
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

        # obj is in this frame
        if self.dist > 0:
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
        # obj not in this frame
        else:
            self.on = False
            self.framesOff += 1
            self.framesOn = 0

    def __str__(self):
        """returns string with all class variables"""
        return ("%s, %s at (%d,%d): dist: %g bearing: %g center: ""(%d,%d) w/h: %g/%g aX/aY: %g/%g, framesOn: %d framesOff: %d on: %s" %
                (Constants.visionObjectTuple[self.visionId],
                 Constants.landmarkTuple[self.localId],
                 self.x, self.y, self.dist, self.bearing,
                 self.centerX, self.centerY,
                 self.width, self.height, self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on))


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


class Crossbar:
    """
    Crossbar class, what used to be the backstop class
    """
    def __init__(self, visionInfos, visionName):
        """initialization all values for FieldObject() class"""
        # Things to be filled out later
        self.framesOn = 0
        self.framesOff = 0
        self.on = False

        # Setup the data from vision
        self.visionId = visionName
        self.updateVision(visionInfos)

    def updateVision(self, visionInfos):
        """updates class variables with new vision information"""
        self.x = visionInfos.x
        self.y = visionInfos.y
        self.angleX = visionInfos.angleX
        self.angleY = visionInfos.angleY
        self.centerX = visionInfos.centerX
        self.centerY = visionInfos.centerY
        self.width = visionInfos.width
        self.height = visionInfos.height
        self.focDist = visionInfos.focDist
        self.distance = visionInfos.distance
        self.bearing = visionInfos.bearing
        self.elevation = visionInfos.elevation
        self.leftOpening = visionInfos.leftOpening
        self.rightOpening = visionInfos.rightOpening
        self.shoot = visionInfos.shoot

        # obj is in this frame
        if self.distance > 0:
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
        # obj not in this frame
        else:
            self.on = False
            self.framesOff += 1
            self.framesOn = 0
