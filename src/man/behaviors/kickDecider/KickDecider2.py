import kicks
import KickingConstants2 as constants
import noggin_constants as nogginC
from objects import Location
import math
import copy
import itertools

# TODO cleaning and documentation
# TODO rethink planner part?
# TODO make completely functional?

# TODO integrate with approach ball
# TODO integrate with motion kicking
class KickDecider2(object):
    """
    The new kick decider.
    """
    def __init__(self, brain):
        self.brain = brain
        self.kicks = [kicks.LEFT_SHORT_STRAIGHT_KICK,
                      kicks.RIGHT_SHORT_STRAIGHT_KICK,
                      kicks.LEFT_SIDE_KICK,
                      kicks.RIGHT_SIDE_KICK]
        self.possibleKicks = self.generateNothing()
    
    ### PLANNERS ###
    def closeToGoal(self):
        self.kicks[0] = kicks.LEFT_SHORT_STRAIGHT_KICK
        self.kicks[1] = kicks.RIGHT_SHORT_STRAIGHT_KICK

        self.clearPossibleKicks()
        self.addShotsOnGoal()
        self.addPassesToFieldCross()

        return (kick for kick in self.possibleKicks).next().next()

    def farFromGoal(self):
        self.kicks[0] = kicks.LEFT_BIG_KICK
        self.kicks[1] = kicks.RIGHT_BIG_KICK

        self.clearPossibleKicks()
        self.addShotsOnGoal()
        self.addPassesToFieldCross()

        return (kick for kick in self.possibleKicks).next().next()

    ### API ###
    def addShotsOnGoal(self):
        x = nogginC.OPP_GOALBOX_RIGHT_X - self.brain.ball.x
        y1 = nogginC.OPP_GOALBOX_MIDDLE_Y - self.brain.ball.y
        y2 = nogginC.OPP_GOALBOX_MIDDLE_Y - constants.SHOT_PRECISION - self.brain.ball.y
        # Two vectors that share an x coordinate but have diff y coordinates
        toGoalCenterMagnitude = math.sqrt(x**2+y1**2)
        toGoalCenterMinusPrecisionMagnitude = math.sqrt(x**2+y2**2)
        # Formula for angle between two vectors, cos(theta) = a.b/||a||||b||
        theta = math.acos((x*x+y1*y2)/(toGoalCenterMagnitude*toGoalCenterMinusPrecisionMagnitude))
        precision = toGoalCenterMagnitude*math.tan(theta)
        if precision >= constants.MIN_PRECISION:
            self.possibleKicks = itertools.chain(self.possibleKicks,
                                                 [self.generateKicksFromGoalDest(nogginC.OPP_GOALBOX_RIGHT_X,
                                                                                 nogginC.CENTER_FIELD_Y,
                                                                                 precision)])

    def addPassesToFieldCross(self):
        betweenFieldCrossAndGoalBoxLeft = (nogginC.LANDMARK_OPP_FIELD_CROSS[0] +
                                           nogginC.OPP_GOALBOX_LEFT_X) / 2.
        self.possibleKicks = itertools.chain(self.possibleKicks,
                                             [self.generateKicksFromGoalDest(betweenFieldCrossAndGoalBoxLeft,
                                                                             nogginC.LANDMARK_OPP_FIELD_CROSS[1],
                                                                             constants.PASS_PRECISION)])

    # def addCrosses(self):
    #     if self.brain.loc.y > nogginC.MIDFIELD_Y:
    #         post = nogginC.LANDMARK_OPP_GOAL_RIGHT_POST_Y
    #     else:
    #         post = nogginC.LANDMARK_OPP_GOAL_LEFT_POST_Y

    #     self.possibleKicks = itertools.chain(self.possibleKicks,
    #                                          [self.generateKicksFromGoalDest(nogginC.LANDMARK_OPP_FIELD_CROSS[0],
    #                                                                          post,
    #                                                                          constants.PASS_PRECISION)])

    # def addClears(self):
    #     self.possibleKicks = itertools.chain.from_iterable(itertools.chain(self.possibleKicks,
    #                                                                        self.generateFastestPossibleKicks()))

    def clearPossibleKicks(self):
        self.possibleKicks = self.generateNothing()

    ### GENERATORS ###
    def generateNothing(self):
        return
        yield

    def generateIdealKicks(self, idealX, idealY, actualX, actualY):
        for k in self.kicks:
            # Two transformations based on similar triangles
            # xOff = kick.setup.x
            # yOff = kick.setup.y
            # x1 = actualX - self.brain.ball.x
            # y1 = actualY - self.brain.ball.y
            # y2 = math.sqrt(xOff**2/(1+(x1**2/y1**2)))
            # x2 = x1*y2/y1
            # x3 = math.sqrt(yOff**2/(1+(x2**2/y2**2)))
            # y3 = x2*x3/y2
            # kick.setup.x = x3
            # kick.setup.y = y3
            # kick.setup.h = math.degrees(math.atan((self.brain.ball.y-y3) /
            #                                       (self.brain.ball.x-x3)))
            # if self.brain.ball.x < x3: kick.setup.h += 180
            kick = copy.deepcopy(k)

            offset = kick.setupH
            kick.setupH = 90 - math.degrees(math.atan(abs(actualX-self.brain.ball.x)/
                                                      abs(actualY-self.brain.ball.y)))
            if actualX < self.brain.ball.x: kick.setupH += 90
            if actualY < self.brain.ball.y: kick.setupH = -kick.setupH
            kick.setupH += offset

            kick.destinationX = idealX
            kick.destinationY = idealY
            yield kick

    def generateFastestPossibleKicks(self):
        fastestKickXDest = 2.*self.brain.ball.x - self.brain.loc.x
        fastestKickYDest = 2.*self.brain.ball.y - self.brain.loc.y
        yield self.generateIdealKicks(fastestKickXDest,fastestKickYDest,
                                      fastestKickXDest,fastestKickYDest)

    # NOTE N should be an odd number, so that a perfectly-aimed kick is chosen
    #      as one of the sampled kicks
    def sampleKicks(self, x, y, precision, N):
        yield self.generateIdealKicks(x,y,x,y)
        # r, thetaInitial = self.fromCartesianToPolarCoordinates(x,y)
        # bigTheta = 2*math.atan(precision/r)
        # littleTheta = bigTheta/N
        # theta = thetaInitial - bigTheta/2.
        # for i in xrange(N):
        #     newX, newY = self.fromPolarToCartesianCoordinates(r,theta)
        #     yield self.generateIdealKicks(x,y,newX,newY)
        #     theta += littleTheta

    def generateKicksFromGoalDest(self, x, y, precision):
        sampledKicks = itertools.chain.from_iterable(self.sampleKicks(x,y,precision,constants.NUM_OF_SAMPLES))

        def scoreKick(kick):
            orbitTime = abs(kick.setupH - self.brain.loc.h)
            return constants.DELTA*orbitTime

        yield max(sampledKicks,key=scoreKick)

    ### FILTERS ###
    # def closeEnough(self, kick):
    #     if kick.destination:
    #         return self.calculateDistanceToBall(kick.destination) < kick.maxDist
    #     return True

    # def inBounds(self, kick):
    #     return  (kick.destination.x >= nogginC.FIELD_WHITE_LEFT_SIDELINE_X and 
    #              kick.destination.x <= nogginC.FIELD_WHITE_RIGHT_SIDELINE_X and
    #              kick.destination.y >= nogginC.FIELD_WHITE_BOTTOM_SIDELINE_Y and
    #              kick.destination.y <= nogginC.FIELD_WHITE_TOP_SIDELINE_Y)

    # def forwardProgress(self, kick):
    #     return (kick.destination.x - self.brain.ball.x > 
    #             constants.FORWARD_ENOUGH_THRES)

    ### HELPER FUNCTIONS ###
    # def calculateKickErrorInY(self, kick):
    #     theta = 180. - kick.setup.headingTo(kick.destination)
    #     return self.calculateDistanceToBall(kick.destination)*math.sin(math.radians(theta))

    # def convertSetupToGlobalCoordinates(self, setup):
    #     globalSetupX = math.cos(setup.h)*setup.x + math.sin(setup.h)*setup.y 
    #     globalSetupY = -math.sin(setup.h)*setup.x + math.cos(setup.h)*setup.y
    #     return Location(globalSetupX,globalSetupY)

    # def calculateDistanceToBall(self, location):
    #     return math.sqrt((location.x - self.brain.ball.x)**2 +
    #                      (location.y - self.brain.ball.y)**2)

    # def scoreAccuracy(self, x):
    #     return math.e**(-constants.ALPHA*x**2)

    # def scoreHeading(self, x):
    #     return math.e**(-constants.BETA*x**2)

    def fromCartesianToPolarCoordinates(self, x, y):
        return math.sqrt(x**2+y**2), math.atan2(y,x)

    def fromPolarToCartesianCoordinates(self, r, theta):
        return r*math.cos(theta), r*math.sin(theta)
