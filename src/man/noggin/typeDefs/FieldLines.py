## All line and corner information for Python

class FieldLines:
    
    def __init__(self, visionInfos):
        self.numCorners = 0
        self.numLines = 0
        self.lines = []
        self.corners = []
        
        self.updateLines(visionInfos)
        self.updateCorners(visionInfos)

    def updateLines(self, visionInfos):
        self.numLines = visionInfos.numLines
        self.lines = visionInfos.linesList

    def updateCorners(self, visionInfos):
        self.numCorners = visionInfos.numCorners
        self.corners = visionInfos.cornersList

class Line:

    def __init__(self, visionLine):
        self.dist = 0
        
        self.updateVision

    def updateVision(self, visionLine):
        self.dist = visionLine.dist

class Corner:

    def __init__(self, visionCorner):
        self.dist = 0
        
        self.updateVision

    def updateVision(self, visionCorner):
        self.dist = visionCorner.dist
        
