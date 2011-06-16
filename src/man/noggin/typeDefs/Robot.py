from .VisualObject import VisualObject

class Robot(VisualObject):
    """
    Holds information about other robots seen by the vision system. Can see
    three of each color (red and navy).
    """

    def __init__(self, visionInfos, botType):
        VisualObject.__init__(self)
        self.topCornerX = 0
        self.topCornerY = 0
        self.elevation = 0

        self.type = botType

    def updateVision(self, visionInfos):
        VisualObject.updateVision(self, visionInfos)
        self.topCornerX = visionInfos.x
        self.topCornerY = visionInfos.y
        self.elevation = visionInfos.elevation
