import kicks
import KickDeciderConstants as constants
import noggin_constants as nogginC
from objects import Location
import math
import copy
import itertools

# TODO hierarchy of lower level planners
# TODO actually use a true array of filters
# TODO calculates a true destination when kicking to goal destination
# TODO kicks.py is ugly and not complete
# TODO use location objects throughout
# TODO player.motionKick flag is unnecessary
# TODO document here and on wiki
class KickDecider(object):
    """
    Decides what kick to do. Load a bunch of kicks into self.kicks, a score 
    function into self.scoreKick, and a bunch of filters into self.filters. 
    Instruct the decider where the kick should go and let it do the math for 
    you. Includes a variety of low level planners for different situations.
    High level planners script the low level planners to create more complicated
    kicking behaviors.
    """
    def __init__(self, brain):
        self.brain = brain

        self.kicks = []                             # array of kicks to try
        self.scoreKick = None                       # score kick function used to choose a kick
        self.filters = []                           # array of filter functions used to filter out kicks

        self.possibleKicks = self.generateNothing() # acceptable kicks ordered by score
    
    ### LOW LEVEL PLANNERS ###
    def sweetMovesOrbit(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)
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
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.addPassesToFieldCross()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def frontKicksOrbit(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        try:
            return (kick for kick in self.possibleKicks).next().next()
        except:
            return None

    def bigKicksOrbit(self):
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
        self.kicks.append(kicks.LEFT_BIG_KICK)
        self.kicks.append(kicks.RIGHT_BIG_KICK)
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            return (kick for kick in self.possibleKicks).next().next()
        except:
            return None

    def motionKicksOrbit(self):
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

    def motionKicksAsapOnGoal(self):
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
        self.filters.append(self.crossesGoalLine)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            return (kick for kick in self.possibleKicks).next().next()
        except:
            return None

    def motionKicksInScrumAsap(self, obstacles):
        self.brain.player.motionKick = True
    
        self.kicks = []
        if not obstacles[1]:
            self.kicks.append(kicks.M_LEFT_SIDE)
            self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        if not obstacles[2]:
            self.kicks.append(kicks.M_RIGHT_SIDE)
            self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)
        if not self.kicks:
            self.kicks.append(kicks.M_LEFT_SIDE)
            self.kicks.append(kicks.M_RIGHT_SIDE)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.inBoundsOrGoalAndForwardProgress)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            return (kick for kick in self.possibleKicks).next().next()
        except:
            return None

    def allKicksAsap(self):
        self.kicks = []
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)
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
            k = (kick for kick in self.possibleKicks).next().next()
            if k.sweetMove: 
                self.brain.player.motionKick = False
            else:
                self.brain.player.motionKick = True
            return k
        except:
            return None

    def sweetMovesForKickOff(self, direction, dest):
        """
        direction 0 is a forward kick, direction 1 is a left side pass
        direction -1 is a right side pass
        """
        self.brain.player.motionKick = False

        self.kicks = []
        if direction == 0:
            self.kicks.append(kicks.LEFT_KICK)
            self.kicks.append(kicks.RIGHT_KICK)
        elif direction == 1:
            self.kicks.append(kicks.RIGHT_SIDE_KICK)
        elif direction == -1:
            self.kicks.append(kicks.LEFT_SIDE_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.clearPossibleKicks()
        self.addPassesTo(dest)

        return (kick for kick in self.possibleKicks).next().next()

    def sweetMoveCrossToCenter(self):
        self.brain.player.motionKick = True

        self.kicks = []
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.clearPossibleKicks()
        self.addPassesToFieldCross()

        return (kick for kick in self.possibleKicks).next().next()

    def allKicksIncludingBigKickAsap(self):
        self.kicks = []
        self.kicks.append(kicks.LEFT_BIG_KICK)
        self.kicks.append(kicks.RIGHT_BIG_KICK)
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)
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
            k = (kick for kick in self.possibleKicks).next().next()
            if k.sweetMove: 
                self.brain.player.motionKick = False
            else:
                self.brain.player.motionKick = True
            return k
        except:
            return None

    def allKicksAsapOnGoal(self):
        self.kicks = []
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            k = (kick for kick in self.possibleKicks).next().next()
            if k.sweetMove: 
                self.brain.player.motionKick = False
            else:
                self.brain.player.motionKick = True
            return k
        except:
            return None

    def allKicksIncludingBigKickAsapOnGoal(self):
        self.kicks = []
        self.kicks.append(kicks.LEFT_BIG_KICK)
        self.kicks.append(kicks.RIGHT_BIG_KICK)
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            k = (kick for kick in self.possibleKicks).next().next()
            if k.sweetMove: 
                self.brain.player.motionKick = False
            else:
                self.brain.player.motionKick = True
            return k
        except:
            return None

    ### HIGH LEVEL PLANNERS ###
    def attacker(self):
        onGoalAsap = self.allKicksAsapOnGoal()
        if onGoalAsap: 
            return onGoalAsap

        asap = self.motionKicksAsap()
        if asap:
            return asap

        return self.frontKickCrosses()

    def defender(self):
        asap = self.allKicksAsap()
        if asap:
            return asap

        return self.frontKickCrosses()

    def obstacleAware(self, clearing = False):
        # motionKicksOnGoal = self.motionKicksAsapOnGoal() # TODO avoid when shooting too?
        # if motionKicksOnGoal:
        #     return motionKicksOnGoal

        # obstacles = [self.checkObstacle(1,75), self.checkObstacle(2,75), self.checkObstacle(8,75)]
        # if True in obstacles:
        #     inScrum = self.motionKicksInScrumAsap(obstacles)
        #     if inScrum:
        #         return inScrum

        if (not self.checkObstacle(1, 150) and 
            not self.checkObstacle(1, 100) and
            not self.checkObstacle(8, 100)): 
            timeAndSpace = self.frontKicksAsapOnGoal() # TODO ask for more precision here?
            if timeAndSpace:
                return timeAndSpace

            # TODO implement smarter clearing strategy
            # if clearing:
            #     timeAndSpace = self.allKicksAsap()
            #     if timeAndSpace:
            #         return timeAndSpace

        asap = self.motionKicksAsap()
        if asap:
            return asap
        
        return self.frontKickCrosses()

    def timeForSomeHeroics(self):
        asapOnGoal = self.allKicksIncludingBigKickAsapOnGoal()
        if asapOnGoal:
            return asapOnGoal

        # TODO hack for Brazil
        goalCenter = Location(nogginC.FIELD_WHITE_RIGHT_SIDELINE_X, nogginC.MIDFIELD_Y)
        ball = Location(self.brain.ball.x, self.brain.ball.y)
        if ball.distTo(goalCenter) <= 200:
            return self.frontKicksOrbit()

        return self.bigKicksOrbit()

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

    def inBoundsOrGoalAndForwardProgress(self, kick):
        return  self.inBoundsOrGoal(kick) and self.forwardProgress(kick)

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

    def forwardProgress(self, kick):
        goalCenter = Location(nogginC.FIELD_WHITE_RIGHT_SIDELINE_X, nogginC.MIDFIELD_Y)
        ball = Location(self.brain.ball.x, self.brain.ball.y)
        kickDestination = Location(kick.destinationX, kick.destinationY)

        return goalCenter.distTo(ball) > goalCenter.distTo(kickDestination)

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

    def checkObstacle(self, position, threshold):
        return (self.brain.obstacles[position] <= threshold and 
                self.brain.obstacles[position] != 0.0)
