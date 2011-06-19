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
        # Lists are wiped every time vision loop executes
        self.lines = []
        for line in visionInfos.linesList:
            self.lines.append(Line(line))

    def updateCorners(self, visionInfos):
        self.numCorners = visionInfos.numCorners
        # Lists are wiped every time vision loop executes
        self.corners = []
        for corner in visionInfos.cornersList:
            self.corners.append(Corner(corner))

#Lines and corners are only created, not updated

class Line:

    def __init__(self, visionLine):
        self.angle = visionLine.angle
        self.width = visionLine.avgWidth
        self.bearing = visionLine.bearing
        self.dist = visionLine.dist
        self.length = visionLine.length
        self.slope = visionLine.slope
        self.yInt = visionLine.yInt
        self.possibilities = visionLine.possibilities

    def __str__(self):
        return "Line with angle " + str(self.angle) + " width " + \
            str(self.width) + " bearing " + str(self.bearing) + " dist " + \
            str(self.dist) + " length " + str(self.length) + " slope " + \
            str(self.slope) + " yInt " + str(self.yInt) + "."

class Corner:

    def __init__(self, visionCorner):
        self.dist = visionCorner.dist
        self.bearing = visionCorner.bearing
        self.x = visionCorner.x
        self.y = visionCorner.y
        self.possibilities = visionCorner.possibilities

    def __str__(self):
        return "Corner with dist " + str(self.dist) + " bearing " + \
            str(self.bearing) + " x " + str(self.x) + " y " + str(self.y) + "."

