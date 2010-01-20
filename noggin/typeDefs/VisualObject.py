
class VisualObject:
    """VisualObject is a class for all objects that need general vision information
    in python. the fields common to all are centerX, centerY, width, height, focDist,
    dist, bearing
    """
    def __init__(self):
        self.centerX = 0
        self.centerY = 0
        self.width = 0
        self.height = 0
        self.focDist = 0
        self.dist = 0
        self.bearing = 0
        self.framesOn = 0
        self.framesOff = 0
        self.on = False

    def updateVision(self, visionInfos):
        self.centerX = visionInfos.centerX
        self.centerY = visionInfos.centerY
        self.width = visionInfos.width
        self.height = visionInfos.height
        self.focDist = visionInfos.focDist
        self.dist = visionInfos.dist
        self.bearing = visionInfos.bearing

        # obj is in this frame
        if self.dist > 0:
            self.on = True
            self.framesOn += 1
            self.framesOff = 0
            """
            ball and fieldObject calculate this here to avoid overhead
            crossbar gets it from c++. is there a reason it needs to?
            self.angleX = (((Constants.IMAGE_WIDTH/2.-1) - self.centerX)/
                           Constants.IMAGE_ANGLE_X)
            self.angleY = (((Constants.IMAGE_HEIGHT/2.-1) - self.centerY)/
                           Constants.IMAGE_ANGLE_Y)
            """
        # obj not in this frame
        else:
            self.on = False
            self.framesOff += 1
            self.framesOn = 0
            """
            self.angleX = 0
            self.angleY = 0
            """

    def __str__(self):
        """returns string with all class variables"""
        return ("dist: %g bearing: %g center: ""(%d,%d) w/h: %g/%g aX/aY: %g/%g \
        framesOn: %d framesOff: %d on: %s " %
                (self.dist, self.bearing,
                 self.centerX, self.centerY,
                 self.width, self.height,
                 self.angleX, self.angleY,
                 self.framesOn, self.framesOff, self.on) )
