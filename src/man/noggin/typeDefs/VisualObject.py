class VisualObject():
    """VisualObject is a class for all objects that need general vision information
    in python. the fields common to all are centerX, centerY, width, height,
    dist, bearing
    """
    def __init__(self):
        self.centerX = 0
        self.centerY = 0
        self.width = 0
        self.height = 0
        self.visDist = 0
        # relative distance according to the vision system
        self.visBearing = 0
        # relative bearing according to the vision system
        self.angleX = 0
        # positive is left from center of vision frame
        self.angleY = 0
        # positive is up from center of vision frame
        self.on = 0
        self.framesOn = 0
        self.framesOff = 0

    def updateVision(self, visionInfos):
        self.centerX = visionInfos.centerX
        self.centerY = visionInfos.centerY
        self.width = visionInfos.width
        self.height = visionInfos.height
        self.visDist = visionInfos.dist
        self.visBearing = visionInfos.bearing
        self.angleX = visionInfos.angleX
        self.angleY = visionInfos.angleY
        self.on = visionInfos.on
        self.framesOn = visionInfos.framesOn
        self.framesOff = visionInfos.framesOff

    def __str__(self):
        """returns string with all class variables"""
        return ("dist: %g bearing: %g center: ""(%d,%d) w/h: %g/%g aX/aY: %g/%g \
        framesOn: %d framesOff: %d on: %s " %
                (self.visDist, self.visBearing,
                 self.centerX, self.centerY,
                 self.width, self.height,
                 self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on) )
