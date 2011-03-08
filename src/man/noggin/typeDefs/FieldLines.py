# Contains Line and Corner information for Python classes.

class FieldLines:
    """
    A Python class that contains all of the information from
    the corresponding PyFieldLines class in PyVision. 
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

