class Sonar:
    """
    Holds the data from the ultrasound sonar sensors
    """
    def __init__(self):
         # Class constants
        self.UNKNOWN_VALUE = -1 #"unknown distance"
        self.MIN_DIST = 4.0 # minimum readable distance in cm
        self.MAX_DIST = 150.0 # maximum readable distance in cm

        self.leftDist = self.UNKNOWN_VALUE
        self.rightDist = self.UNKNOWN_VALUE

        self.hasPrintedError = False

    def updateSensors(self, sensors):
        """
        Update the sonar info from the most recent sensors
        """
        self.leftDist = sensors.ultraSoundDistanceLeft
        self.rightDist = sensors.ultraSoundDistanceRight

        if (self.leftDist <= self.MIN_DIST or
            self.leftDist >= self.MAX_DIST):
            self.leftDist = self.UNKNOWN_VALUE

        if (self.rightDist <= self.MIN_DIST or
            self.rightDist >= self.MAX_DIST):
            self.rightDist = self.UNKNOWN_VALUE


    def __str__(self):
        return ("left sonar has distance of " + str(self.leftDist) +
                " right sonar has a distance of " + str(self.rightDist))
