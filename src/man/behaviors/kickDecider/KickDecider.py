import kicks
import KickDeciderConstants as constants
import noggin_constants as nogginC
from objects import Location
import math
import copy
import itertools

# TODO use robot and open field detection to decide kicks
# TODO develop kickoff strategy

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
        self.kicks = []
        self.kicks.append(kicks.LEFT_SHORT_STRAIGHT_KICK)
        self.kicks.append(kicks.RIGHT_SHORT_STRAIGHT_KICK)
        self.kicks.append(kicks.LEFT_SIDE_KICK)
        self.kicks.append(kicks.RIGHT_SIDE_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def bigKicksOnGoal(self):
        self.kicks = []
        self.kicks.append(kicks.LEFT_BIG_KICK)
        self.kicks.append(kicks.RIGHT_BIG_KICK)
        self.kicks.append(kicks.LEFT_SIDE_KICK)
        self.kicks.append(kicks.RIGHT_SIDE_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def motionKicksOnGoal(self):
        self.brain.player.motionKick = True
    
        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        self.kicks.append(kicks.M_LEFT_SIDE_OUT)
        self.kicks.append(kicks.M_RIGHT_SIDE_OUT)

        self.scoreKick = self.minimizeOrbitTime

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
        self.kicks.append(kicks.M_LEFT_SIDE_OUT)
        self.kicks.append(kicks.M_RIGHT_SIDE_OUT)

        self.scoreKick = self.minimizeDistanceToGoal

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        # TODO add filter for out of bounds balls
        return (kick for kick in self.possibleKicks).next()

    def motionKicks(self):
        if self.brain.ball.distTo(Location(nogginC.OPP_GOALBOX_RIGHT_X,
                                           nogginC.CENTER_FIELD_Y)) < constants.SHOT_THRESHOLD:
            return self.motionKicksOnGoal()
        return self.motionKicksAsap()

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
                                             [self.generateKicksFromGoalDest(nogginC.OPP_GOALBOX_RIGHT_X,
                                                                             nogginC.CENTER_FIELD_Y,
                                                                             precision)])

    def addPassesTo(self, location):
        self.possibleKicks = itertools.chain(self.possibleKicks,
                                             [self.generateKicksFromGoalDest(location.x,
                                                                             location.y,
                                                                             constants.PASS_PRECISION)])

    def addPassesToFieldCross(self):
        betweenFieldCrossAndGoalBoxLeft = (nogginC.LANDMARK_OPP_FIELD_CROSS[0] +
                                           nogginC.OPP_GOALBOX_LEFT_X) / 2.
        location = Location(betweenFieldCrossAndGoalBoxLeft,nogginC.LANDMARK_OPP_FIELD_CROSS[1])


        self.addPassesTo(location)

    def addFastestPossibleKicks(self):
        self.possibleKicks = itertools.chain.from_iterable(itertools.chain(self.possibleKicks,
                                                                           self.generateFastestPossibleKicks()))

    def clearPossibleKicks(self):
        self.possibleKicks = self.generateNothing()

    ### GENERATORS ###
    def generateNothing(self):
        return
        yield

    def generateIdealKicks(self, x, y):
        for k in self.kicks:
            kick = copy.deepcopy(k)

            offset = kick.setupH
            kick.setupH = 90 - math.degrees(math.atan(abs(x-self.brain.ball.x)/
                                                      abs(y-self.brain.ball.y)))
            if x < self.brain.ball.x: kick.setupH += 90
            if y < self.brain.ball.y: kick.setupH = -kick.setupH
            kick.setupH += offset

            kick.destinationX = x
            kick.destinationY = y
            yield kick

    def generateFastestPossibleKicks(self):
        fastestKickXDest = 2.*self.brain.ball.x - self.brain.loc.x
        fastestKickYDest = 2.*self.brain.ball.y - self.brain.loc.y
        yield self.generateIdealKicks(fastestKickXDest,fastestKickYDest)

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

        yield max(sampledKicks,key=self.scoreKick)

    ### SCORE KICK FUNCTIONS ###
    def minimizeOrbitTime(self, kick):
        orbitTime = abs(kick.setupH - self.brain.loc.h)

        return -orbitTime

    def minimizeDistanceToGoal(self, kick):
        goalCenter = Location(nogginC.OPP_GOALBOX_RIGHT_X,
                              nogginC.CENTER_FIELD_Y)
        
        return math.sqrt((kick.destinationX - goalCenter.x)**2 +
                         (kick.destinationY - goalCenter.y)**2)

    ### FILTERS ###
    def inBounds(self, kick):
        return  (kick.destinationX >= nogginC.FIELD_WHITE_LEFT_SIDELINE_X and 
                 kick.destinationX <= nogginC.FIELD_WHITE_RIGHT_SIDELINE_X and
                 kick.destinationY >= nogginC.FIELD_WHITE_BOTTOM_SIDELINE_Y and
                 kick.destinationY <= nogginC.FIELD_WHITE_TOP_SIDELINE_Y)

    ### HELPER FUNCTIONS ###
    def fromCartesianToPolarCoordinates(self, x, y):
        return math.sqrt(x**2+y**2), math.atan2(y,x)

    def fromPolarToCartesianCoordinates(self, r, theta):
        return r*math.cos(theta), r*math.sin(theta)
