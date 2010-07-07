from .. import NogginConstants as Constants

class Stability:
    """
    Collects stability data measured by accelerometers
    """

    def __init__(self, sensors):
        self.sensors = sensors

        self.accelX = []
        self.accelY = []

        return

    def resetData(self):
        del self.accelX[:]
        del self.accelY[:]
        return

    def updateStability(self):
        inertial = self.sensors.inertial

        self.accelX.append(inertial.accX)
        self.accelY.append(inertial.accY)

        return

    def getStability_X(self):
        return self.calculateStabilityVariance(self.accelX)

    def getStability_Y(self):
        return self.calculateStabilityVariance(self.accelY)

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
