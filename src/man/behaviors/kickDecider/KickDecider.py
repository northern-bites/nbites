import kicks
import KickDeciderConstants as constants
import noggin_constants as nogginC
from objects import Location
import math
import copy
import itertools

# TODO use open field detection to decide kicks

# TODO document here and on wiki
# TODO kicks.py is ugly
# TODO abstract from behaviors for testing purposes
class KickDecider(object):
    """
    Decides what kick to do. Load a bunch of kicks into self.kicks, a score 
    function into self.scoreKick, and a bunch of filters into self.filters. 
    Instruct the decider where the kick should go and let it do the math for 
    you. Includes a variety of ready to go planners for different situations. 
    """
    def __init__(self, brain):
        self.brain = brain

        self.kicks = []                             # array of kicks to try
        self.scoreKick = None                       # score kick function used to choose a kick
        self.filters = []                           # array of filter functions used to filter out kicks

        self.possibleKicks = self.generateNothing() # acceptable kicks ordered by utility
    
    ### PLANNERS ###
    def sweetMovesOnGoal(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.LEFT_SHORT_STRAIGHT_KICK)
        self.kicks.append(kicks.RIGHT_SHORT_STRAIGHT_KICK)
        self.kicks.append(kicks.LEFT_SIDE_KICK)
        self.kicks.append(kicks.RIGHT_SIDE_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def frontKickCrosses(self):
        self.brain.player.motionKick = False
        
        self.kicks = []
        self.kicks.append(kicks.LEFT_SHORT_STRAIGHT_KICK)
        self.kicks.append(kicks.RIGHT_SHORT_STRAIGHT_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.addPassesToFieldCross()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def bigKicksOnGoal(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.LEFT_BIG_KICK)
        self.kicks.append(kicks.RIGHT_BIG_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def frontKicksAsapOnGoal(self):
        self.brain.player.motionKick = False
    
        self.kicks = []
        self.kicks.append(kicks.LEFT_SHORT_STRAIGHT_KICK)
        self.kicks.append(kicks.RIGHT_SHORT_STRAIGHT_KICK)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            return (kick for kick in self.possibleKicks).next().next()
        except:
            return None

    def motionKicksOnGoal(self):
        self.brain.player.motionKick = True
    
        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def motionKicksAsap(self):
        self.brain.player.motionKick = True
    
        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.inBoundsOrGoal)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            return (kick for kick in self.possibleKicks).next().next()
        except:
            return None

    def brunswick(self):
        onGoalAsap = self.frontKicksAsapOnGoal()
        if onGoalAsap: 
            return onGoalAsap

        asap = self.motionKicksAsap()
        if asap:
            return asap

        return self.frontKickCrosses()

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
        self.possibleKicks = itertools.chain(self.possibleKicks,
                                             self.generateKicksFromGoalDest(nogginC.OPP_GOALBOX_RIGHT_X,
                                                                            nogginC.CENTER_FIELD_Y,
                                                                            precision))

    def addPassesTo(self, location):
        self.possibleKicks = itertools.chain(self.possibleKicks,
                                             self.generateKicksFromGoalDest(location.x,
                                                                            location.y,
                                                                            constants.PASS_PRECISION))

    def addPassesToFieldCross(self):
        betweenFieldCrossAndGoalBoxLeft = (nogginC.LANDMARK_OPP_FIELD_CROSS[0] +
                                           nogginC.OPP_GOALBOX_LEFT_X) / 2.
        location = Location(betweenFieldCrossAndGoalBoxLeft,nogginC.CENTER_FIELD_Y)

        self.addPassesTo(location)

    def addFastestPossibleKicks(self):
        self.possibleKicks = itertools.chain(self.possibleKicks,
                                             self.generateFastestAndHighestScoringKick())

    def clearPossibleKicks(self):
        self.possibleKicks = self.generateNothing()

    ### GENERATORS ###
    def generateNothing(self):
        return
        yield

    def filterAndScoreKicks(self, kicks):
        if self.filters:
            for filt in self.filters:
                filteredKicks = (kick for kick in kicks if filt(kick))
        else:
            filteredKicks = kicks

        try:
            yield max(filteredKicks,key=self.scoreKick)
        except ValueError:
            yield filteredKicks.next()

    def generateFastestPossibleKicks(self):
        for k in self.kicks:
            kick = copy.deepcopy(k)

            offset = -kick.setupH
            kick.setupH = self.brain.loc.h

            # Uses two rotation matrices to calculate where the kick will go
            beforeFirstRotationX = kick.distance

            afterFirstRotationX = beforeFirstRotationX*math.cos(math.radians(kick.setupH))
            afterFirstRotationY = beforeFirstRotationX*math.sin(math.radians(kick.setupH))

            afterSecondRotationX = (afterFirstRotationX*math.cos(math.radians(offset)) -
                                    afterFirstRotationY*math.sin(math.radians(offset)))
            afterSecondRotationY = (afterFirstRotationX*math.sin(math.radians(offset)) +
                                    afterFirstRotationY*math.cos(math.radians(offset)))

            kick.destinationX = self.brain.ball.x + afterSecondRotationX
            kick.destinationY = self.brain.ball.y + afterSecondRotationY

            yield kick

    def generateFastestAndHighestScoringKick(self):
        fastestKicks = self.generateFastestPossibleKicks()

        yield self.filterAndScoreKicks(fastestKicks)

    def generateIdealKicks(self, x, y):
        for k in self.kicks:
            kick = copy.deepcopy(k)
            
            kickDestinationX = x - self.brain.ball.x
            kickDestinationY = y - self.brain.ball.y

            # Angle between unit vector and kickDestination, cos(theta) = a.b / ||a||||b||
            angleToKickDestination = math.degrees(math.acos(kickDestinationX / 
                                                            math.sqrt(kickDestinationX**2 + kickDestinationY**2)))
            if kickDestinationY < 0: angleToKickDestination = -angleToKickDestination

            # Before the following line, kick.setupH holds offset from standard kick setup
            kick.setupH = self.normalizeAngle(angleToKickDestination + kick.setupH)
            # After, kick.setupH holds global heading used for kick allignment

            kick.destinationX = x
            kick.destinationY = y

            yield kick

    # NOTE N should be an odd number, so that a perfectly-aimed kick is chosen
    #      as one of the sampled kicks
    def sampleKicks(self, x, y, precision, N):
        if constants.NO_SAMPLING:
            yield self.generateIdealKicks(x,y)
        else:
            r, thetaInitial = self.fromCartesianToPolarCoordinates(x-self.brain.ball.x,
                                                                   y-self.brain.ball.y)
            bigTheta = 2*math.atan(precision/r)
            littleTheta = bigTheta/N
            theta = thetaInitial - bigTheta/2.
            for i in xrange(N):
                newX, newY = self.fromPolarToCartesianCoordinates(r,theta)
                yield self.generateIdealKicks(newX+self.brain.ball.x,
                                              newY+self.brain.ball.y)
                theta += littleTheta

    def generateKicksFromGoalDest(self, x, y, precision):
        sampledKicks = itertools.chain.from_iterable(self.sampleKicks(x,y,precision,constants.NUM_OF_SAMPLES))

        yield self.filterAndScoreKicks(sampledKicks)

    ### SCORE KICK FUNCTIONS ###
    def minimizeOrbitTime(self, kick):
        orbitTime = abs(self.normalizeAngle(kick.setupH - self.brain.loc.h))
        return -orbitTime

    def minimizeDistanceToGoal(self, kick):
        if self.crossesGoalLine(kick):
            offset = 10000000 # TODO make less of a hack
        else:
            offset = 0

        goalCenter = Location(nogginC.OPP_GOALBOX_RIGHT_X,
                              nogginC.CENTER_FIELD_Y)
        return (offset - math.sqrt((kick.destinationX - goalCenter.x)**2 +
                                   (kick.destinationY - goalCenter.y)**2))

    ### FILTERS ###
    def inBoundsOrGoal(self, kick):
        return self.inBounds(kick) or self.crossesGoalLine(kick)

    def inBounds(self, kick):
        onField = (kick.destinationX >= nogginC.FIELD_WHITE_LEFT_SIDELINE_X and 
                   kick.destinationX <= nogginC.FIELD_WHITE_RIGHT_SIDELINE_X and
                   kick.destinationY >= nogginC.FIELD_WHITE_BOTTOM_SIDELINE_Y and
                   kick.destinationY <= nogginC.FIELD_WHITE_TOP_SIDELINE_Y)

        return onField

    def crossesGoalLine(self, kick):
        if self.brain.ball.x > nogginC.FIELD_WHITE_RIGHT_SIDELINE_X:
            ballX = nogginC.FIELD_WHITE_RIGHT_SIDELINE_X - 1
        else:
            ballX = self.brain.ball.x

        kickVector = [kick.destinationX - ballX,
                      kick.destinationY - self.brain.ball.y]
        goalLineX = nogginC.FIELD_WHITE_RIGHT_SIDELINE_X - ballX
        rightPostY = nogginC.LANDMARK_OPP_GOAL_RIGHT_POST_Y - self.brain.ball.y 
        leftPostY = nogginC.LANDMARK_OPP_GOAL_LEFT_POST_Y - self.brain.ball.y 

        scale = goalLineX / kickVector[0]

        if 0 <= scale <= 1:
            return (rightPostY <= kickVector[1]*scale <= leftPostY)
        return False

    ### HELPER FUNCTIONS ###
    def fromCartesianToPolarCoordinates(self, x, y):
        return math.sqrt(x**2+y**2), math.atan2(y,x)

    def fromPolarToCartesianCoordinates(self, r, theta):
        return r*math.cos(theta), r*math.sin(theta)

    def normalizeAngle(self, angle):
        newAngle = angle
        while newAngle <= -180: newAngle += 360
        while newAngle > 180: newAngle -= 360
        return newAngle
