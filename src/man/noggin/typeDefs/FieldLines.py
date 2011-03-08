# Contains Line and Corner information for Python classes.

class FieldLines:
    """
    A Python class that contains all of the information from
    the corresponding PyFieldLines class in PyVision. Contains:
    numLines = number of lines in the image
    lines = a list of lines in the image. Note that these lines are
    not the "Line" class included later but can be used as a 
    template to create a Line
    numCorners = number of corners in the image
    corners = a list of corners in the image. Note: same as for lines
    """

    def __init__(self, visionInfos):
        # Initialize all the info
        self.numLines = 0
        self.lines = []
        self.numCorners = 0
        self.corners = []

        # Fill it in
        self.updateVision(visionInfos)

    def updateVision(self, visionInfos):
        """
        Update all of the info based on a PyVision fieldLines object
        """
        self.numLines = visionInfos.numLines
        self.lines = visionInfos.lines
        self.numCorners =  visionInfos.numCorners
        self.corners = visionInfos.corners

    def __str__(self): 
        """toString"""
        return "There are " + str(self.numLines) + " lines."

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
        self.x1 = 0
        self.y1 = 0
        self.x2 = 0
        self.y2 = 0
        self.slope = 0
        self.length = 0
        self.update

    def update(self, visionLine):
        self.x1 = visionLine.x1
        self.y1 = visionLine.y1
        self.x2 = visionLine.x2
        self.y2 = visionLine.y2
        self.slope = visionLine.slope
        self.length = visionLine.length

    def __str__(self):
        return ("p1: (%g,%g) p2: (%g,%g) slope: %g length: %g" %
                (self.x1,self.y1,self.x2,self.y1,self.slope,self.length))

class VisualCorner:
    """
    Corresponds to PyVisualCorner, contains:
    - dist -- distance to the center of the corner
    - bearing -- bearing to the center of the corner
    - possibilities -- list of possible ConcreteCorners it could be
    """
    def __init__(self):
        self.dist = 0
        self.bearing = 0
        self.possibilities = []
        self.update

    def update(self, visionCorner):
        self.dist = visionCorner.dist
        self.bearing = visionCorner.bearing
        self.possibilities = visionCorner.possibilities

    def __str__(self):
        return ("dist: %g bearing: %g" % (self.dist, self.bearing))




