## Python object that holds information from VisualFieldEdge in C++

class FieldEdge:

    def __init__(self, visionInfos):
        self.maxDist = 0
        self.leftDist = 0
        self.rightDist = 0
        self.centerDist = 0
        self.shape = 0

        self.updateVision(visionInfos)

    def updateVision(self, visionInfos):
        self.maxDist = visionInfos.maxDist
        self.leftDist = visionInfos.leftDist
        self.rightDist = visionInfos.rightDist
        self.centerDist = visionInfos.centerDist
        self.shape = visionInfos.shape

    def __str__(self):
        return "Field edge has max distance: " + str(self.maxDist) + \
            " left distance: " + str(self.leftDist) + \
            " right distance: " + str(self.rightDist) + \
            " center distance: " + str(self.centerDist) + \
            " shape: " + str(self.shape) + "."
