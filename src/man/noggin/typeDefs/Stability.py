from .. import NogginConstants as Constants
from math import fabs

FALL_ACCZ_THRESHOLD = 5
FALL_FRAMES_THRESHOLD = 15

# unstable walks that do not fall have accelerometer variance
# on the order of 1.5-3, stable walks are < 1
X_STABILITY_WEIGHT = 100
Y_STABILITY_WEIGHT = 100

class Stability:
    """
    Collects stability data measured by accelerometers
    """

    def __init__(self, sensors):
        self.sensors = sensors

        self.accelX = []
        self.accelY = []

        self.fallCounter = 0

        return

    def resetData(self):
        del self.accelX[:]
        del self.accelY[:]
        self.fallCounter = 0

    def updateStability(self):
        inertial = self.sensors.inertial

        self.accelX.append(inertial.accX)
        self.accelY.append(inertial.accY)

        return

    def isWBFallen(self):
        inertial = self.sensors.inertial

        if fabs(inertial.accZ) < FALL_ACCZ_THRESHOLD:
            self.fallCounter += 1
        else:
            self.fallCounter = 0

        if self.fallCounter > FALL_FRAMES_THRESHOLD:
            return True
        else:
            return False

    def getStability_X(self):
        return self.calculateStabilityVariance(self.accelX)

    def getStability_Y(self):
        return self.calculateStabilityVariance(self.accelY)

    def getStabilityHeuristic(self):
        xStabilityHeuristic = X_STABILITY_WEIGHT * self.getStability_X()
        yStabilityHeuristic = Y_STABILITY_WEIGHT * self.getStability_Y()

        return xStabilityHeuristic + yStabilityHeuristic

    def calculateStabilityVariance(self, data):
        sampleMean = self.calculateSampleMean(data)
        sumSquares = 0
        n = 0

        for x in data:
            n += 1
            sumSquares += (x - sampleMean) ** 2

        if n == 0:
            return 0

        return sumSquares/n

    def calculateSampleMean(self, data):
        n = 0
        sum = 0

        for x in data:
            n += 1
            sum += x

        if n == 0:
            return 0

        return sum/n
