
class Sonar:
    """
    Holds the data from the ultrasound sonar sensors
    """
    def __init__(self):
        # Class constants
        self.UNKNOWN_VALUE = "unknown distance"
        self.MIN_DIST = 0.0 # minimum readable distance in cm
        self.MAX_DIST = 254.0 # maximum readable distance in cm

        self.lastDist = self.UNKNOWN_VALUE
        self.lastMode = None

        self.LLdist = self.UNKNOWN_VALUE
        self.RRdist = self.UNKNOWN_VALUE
        self.LRdist = self.UNKNOWN_VALUE
        self.RLdist = self.UNKNOWN_VALUE

    def updateSensors(self, sensors, modes):
        """
        Update the sonar info from the most recent sensors
        """
        self.lastDist = sensors.ultraSoundDistance
        self.lastMode = sensors.ultraSoundMode

        if (self.lastDist <= self.MIN_DIST or
            self.lastDist >= self.MAX_DIST):
            self.lastDist = self.UNKNOWN_VALUE
        if self.lastMode == modes.LL:
            self.LLdist = self.lastDist
        elif self.lastMode == modes.RR:
            self.RRdist = self.lastDist
        elif self.lastMode == modes.LR:
            self.LRdist = self.lastDist
        elif self.lastMode == modes.RL:
            self.RLdist = self.lastDist

    def __str__(self):
        return ("Last reading of " + str(self.lastMode) +
                " has distance of " + str(self.lastDist) +
                " \n" +
                "LL dist: " + str(self.LLdist) + "\n" +
                "RR dist: " + str(self.RRdist) + "\n" +
                "LR dist: " + str(self.LRdist) + "\n" +
                "RL dist: " + str(self.RLdist))
