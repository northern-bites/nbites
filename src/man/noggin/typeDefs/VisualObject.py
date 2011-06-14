from .. import NogginConstants as Constants
from .Location import Location

class VisualObject(Location):
    """VisualObject is a class for all objects that need general vision information
    in python. the fields common to all are centerX, centerY, width, height, 
    dist, bearing
    """
    def __init__(self):
        Location.__init__(self, 0,0,0)
        self.centerX = 0
        self.centerY = 0
        self.width = 0
        self.height = 0
        self.visDist = 0
        self.visBearing = 0
        self.framesOn = 0
        self.framesOff = 0
        self.on = False

    def updateVision(self, visionInfos):
        self.centerX = visionInfos.centerX
        self.centerY = visionInfos.centerY
        self.width = visionInfos.width
        self.height = visionInfos.height
        self.visDist = visionInfos.dist
        self.visBearing = visionInfos.bearing
        self.angleX = visionInfos.angleX
        self.angleY = visionInfos.angleY

        # obj is in this frame
        if self.visDist > 0:
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
        return ("dist: %g bearing: %g center: ""(%d,%d) w/h: %g/%g aX/aY: %g/%g \
        framesOn: %d framesOff: %d on: %s " %
                (self.visDist, self.visBearing,
                 self.centerX, self.centerY,
                 self.width, self.height,
                 self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on) )
