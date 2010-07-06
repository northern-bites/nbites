from .. import NogginConstants as Constants

class Stability:
    """
    Collects stability data measured by accelerometers
    """

    def __init__(self, sensors):
        self.sensors = sensors
        return


    def updateStability(self):
        return

    def isStanding(self):
        return True
