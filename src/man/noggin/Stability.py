from math import fabs

try:
    from numpy import corrcoef
    haveNumpy = True
except:
    haveNumpy = False
    print "could not load numpy, please install it for advanced statistics"

# unstable walks that do not fall have accelerometer variance
# on the order of 1.5-3, stable walks are < 1
X_STABILITY_WEIGHT = 50
Y_STABILITY_WEIGHT = 50
XY_CORRELATION_WEIGHT = 250

class Stability:
    """
    Collects stability data measured by accelerometers
    and checks to see how linear our path was
    """
    def __init__(self, sensors):
        self.sensors = sensors

        self.accelX = []
        self.accelY = []
        self.pastPositions = []

        self.fallCounter = 0

    def resetData(self):
        del self.accelX[:]
        del self.accelY[:]
        del self.pastPositions[:]
        self.fallCounter = 0

    def updateStability(self):
        inertial = self.sensors.inertial

        self.accelX.append(inertial.accX)
        self.accelY.append(inertial.accY)

    def updatePosition(self, currentLocation):
        self.pastPositions.append(currentLocation)

    def getStability_X(self):
        return self.calculateStabilityVariance(self.accelX)

    def getStability_Y(self):
        return self.calculateStabilityVariance(self.accelY)

    def getStabilityHeuristic(self):
        '''
        Calculates the variance of the accelerometers in X & Y, multiplies them
        by a constant weight to bring them above zero and then returns the sum
        of the values squared.
        '''

        xVariance = self.getStability_X()
        yVariance = self.getStability_Y()

        xStabilityHeuristic = X_STABILITY_WEIGHT * xVariance
        yStabilityHeuristic = Y_STABILITY_WEIGHT * yVariance

        print 'X/Y accelerometer variance: {0:.3f}/{1:.3f}'.format(xVariance, yVariance)

        return xStabilityHeuristic**2 + yStabilityHeuristic**2

    def getPathLinearity(self):
        # sanity check so we don't crash when running on the Nao
        if not haveNumpy:
            return 0

        xPositions, yPositions = self.getPositionsFromPath()

        corr = corrcoef([xPositions, yPositions])

        print "X/Y path correlation is ", corr[0][1]

        # we only care about how linear the path is, not the direction
        return fabs(corr[0][1]) * XY_CORRELATION_WEIGHT

    def getPositionsFromPath(self):
        xPos = []
        yPos = []

        for location in self.pastPositions:
            xPos.append(location.x)
            yPos.append(location.y)

        return xPos, yPos

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
