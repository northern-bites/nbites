import kicks
import KickDeciderConstants as constants
import noggin_constants as nogginC
from objects import Location
import math
import copy
import itertools

# TODO China 2015
#      this file has gotten messy over the years, the general design is fairly good, I think,
#      but someone needs to refactor/clean up all the code that is no longer used,
#      so the entire decider can fit inside someone's head

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
        self.kicks.append(kicks.LEFT_STRAIGHT_KICK)
        self.kicks.append(kicks.RIGHT_STRAIGHT_KICK)
        # self.kicks.append(kicks.LEFT_SIDE_KICK)
        # self.kicks.append(kicks.RIGHT_SIDE_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        return (kick for kick in self.possibleKicks).next().next()

    def frontKickCrosses(self):
        self.brain.player.motionKick = True
        
        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.addPassesToFieldCross()
        self.addShotsOnGoal()

        kickReturned =  (kick for kick in self.possibleKicks).next().next()

        return kickReturned

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

    def frontKicksOrbitIfSmall(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.LEFT_MEDIUM_STRAIGHT_KICK)
        self.kicks.append(kicks.RIGHT_MEDIUM_STRAIGHT_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []
        self.filters.append(self.isShortOrbit)

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

    def frontKicksClear(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.LEFT_KICK)
        self.kicks.append(kicks.RIGHT_KICK)

        self.scoreKick = self.minimizeDistanceToGoal

        self.filters = []
        self.filters.append(self.upfieldEnough)

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
            var = (kick for kick in self.possibleKicks).next().next()
            return var
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
        if self.brain.obstacles[2] == 0 or self.brain.obstacles[8] == 0:
            if self.brain.obstacles[2] == 0:
                self.kicks.append(kicks.M_LEFT_SIDE)
                self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
            else:
                self.kicks.append(kicks.M_RIGHT_SIDE)
                self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)
        else:
            if self.brain.obstacles[2] > self.brain.obstacles[8]:
                self.kicks.append(kicks.M_LEFT_SIDE)
                self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
            else:
                self.kicks.append(kicks.M_RIGHT_SIDE)
                self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.inBoundsOrGoalAndForwardProgress)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            return (kick for kick in self.possibleKicks).next().next()
        except:
            return None

    def kicksBeforeBallIsFree(self):
        asap = self.motionKicksAsap()
        if asap:
            return asap

        return self.frontKickCrosses()

    def kickOutOfBounds(self):
        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        # self.kicks.append(kicks.LEFT_MEDIUM_STRAIGHT_KICK)
        # self.kicks.append(kicks.RIGHT_MEDIUM_STRAIGHT_KICK)

        self.filters = []
        self.filters.append(self.outOfBounds)

        self.scoreKick = self.minimizeKickTime

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

    def allKicksAsap(self):
        self.kicks = []
        # self.kicks.append(kicks.LEFT_MEDIUM_STRAIGHT_KICK)
        # self.kicks.append(kicks.RIGHT_MEDIUM_STRAIGHT_KICK)
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.inBoundsOrGoal)
        self.filters.append(self.notTowardOurGoal)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            k = (kick for kick in self.possibleKicks).next().next()
            # print "kick being used: ", k
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

    def motionKickOnGoal(self):
        self.brain.player.motionKick = True

        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.addShotsOnBackOfGoal()

        try:
            kickReturned =  (kick for kick in self.possibleKicks).next().next()
            return kickReturned
        except:
            return None

    def forwardKickOnGoal(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.BH_LEFT_FORWARD_KICK)
        self.kicks.append(kicks.BH_RIGHT_FORWARD_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.addShotsOnBackOfGoal()

        try:
            kickReturned =  (kick for kick in self.possibleKicks).next().next()
            return kickReturned
        except:
            return None

    def timeForSomeHeroics(self):
        self.brain.player.motionKick = False

        self.kicks = []
        self.kicks.append(kicks.BH_LEFT_FORWARD_KICK)
        self.kicks.append(kicks.BH_RIGHT_FORWARD_KICK)

        self.scoreKick = self.minimizeOrbitTime

        self.filters = []

        self.addShotsOnBackOfGoal()

        try:
            kickReturned =  (kick for kick in self.possibleKicks).next().next()
            return kickReturned
        except:
            return None

    def allKicksAsapOnGoal(self):
        # print "In allKicksAsapOnGoal"

        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_SIDE)
        self.kicks.append(kicks.M_RIGHT_SIDE)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)
        # self.kicks.append(kicks.LEFT_MEDIUM_STRAIGHT_KICK)
        # self.kicks.append(kicks.RIGHT_MEDIUM_STRAIGHT_KICK)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.clearPossibleKicks()
        self.addFastestPossibleKicks()

        try:
            k = (kick for kick in self.possibleKicks).next().next()
            # print "Kick being used: ", k
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

    def frontKicksOnGoal(self):
        self.kicks = []
        self.kicks.append(kicks.M_LEFT_STRAIGHT)
        self.kicks.append(kicks.M_RIGHT_STRAIGHT)
        self.kicks.append(kicks.M_LEFT_CHIP_SHOT)
        self.kicks.append(kicks.M_RIGHT_CHIP_SHOT)
        self.kicks.append(kicks.LEFT_MEDIUM_STRAIGHT_KICK)
        self.kicks.append(kicks.RIGHT_MEDIUM_STRAIGHT_KICK)

        self.scoreKick = self.minimizeDistanceToGoal
        
        self.filters = []
        self.filters.append(self.crossesGoalLine)

        self.clearPossibleKicks()
        self.addShotsOnGoal()

        try:
            k = (kick for kick in self.possibleKicks).next().next()
            if k.sweetMove: 
                self.brain.player.motionKick = False
            else:
                self.brain.player.motionKick = True
            return k
        except:
            return None
    
    ###########################
    ### HIGH LEVEL PLANNERS ###
    ###########################
    def decidingStrategy(self):
        closeGoalShot = self.motionKickOnGoal()
        if closeGoalShot:
            return closeGoalShot

        goalShot = self.forwardKickOnGoal()
        if goalShot:
            return goalShot

        asap = self.motionKicksAsap()
        if asap:
            return asap
        
        return self.frontKickCrosses()

    # not currently used
    def nearOurGoal(self):
        nearOurGoal = (math.fabs(self.brain.loc.h) > 100 and 
                       self.brain.ball.x < nogginC.LANDMARK_BLUE_GOAL_CROSS_X)
        if nearOurGoal:
            out = self.kickOutOfBounds()
            if out:
                self.brain.player.kickedOut = True
                return out
            else:
                return None
        else:
            return None

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

    def addShotsOnBackOfGoal(self):
        x = nogginC.OPP_GOALBOX_RIGHT_X - self.brain.ball.x + 50 
        y1 = nogginC.OPP_GOALBOX_MIDDLE_Y - self.brain.ball.y
        y2 = nogginC.OPP_GOALBOX_MIDDLE_Y - constants.SHOT_PRECISION - self.brain.ball.y
        # Two vectors that share an x coordinate but have diff y coordinates
        toGoalCenterMagnitude = math.sqrt(x**2+y1**2)
        toGoalCenterMinusPrecisionMagnitude = math.sqrt(x**2+y2**2)
        # Formula for angle between two vectors, cos(theta) = a.b/||a||||b||
        theta = math.acos((x*x+y1*y2)/(toGoalCenterMagnitude*toGoalCenterMinusPrecisionMagnitude))
        precision = toGoalCenterMagnitude*math.tan(theta)
        self.possibleKicks = itertools.chain(self.possibleKicks,
                                             self.generateKicksFromGoalDest(nogginC.OPP_GOALBOX_RIGHT_X + 50,
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
        filteredKickLists = []
        if self.filters:
            for filt in self.filters:
                # print "Filter tested: ", filt
                filtKick = [kick for kick in kicks if filt(kick)]
                filteredKickLists.append(filtKick)

            # print "Current list of filters: ", filteredKickLists
            filteredKicks = list(set.intersection(*map(set, filteredKickLists)))
            # print "Filtered kicks: ", filteredKicks
        else:
            filteredKicks = kicks

        try:
            tempVar = max(filteredKicks,key=self.scoreKick)
            # print "This is the max scored kick", tempVar
            yield tempVar
        except ValueError:
            # print "No kick satisfied the filter req's"
            yield filteredKicks #list is empty
            #try:
            #    varIter = iter(filteredKicks)
            #    var = varIter.next()
            #    print "Sending kick: ", var
            #    yield var
            #except StopIteration:
            #    print "Empty list in the filter function"
            #    yield  var

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
            # Our unit vector is (1,0). We are trying to determine our angle wre to the normal.
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
        # print "Orbit time: ", -orbitTime
        return -orbitTime

    def minimizeKickTime(self, kick):
        if kick.sweetMove:
            t = 1
        else:
            t = 2   # prioritize motion kicks
        
        return t

    def minimizeDistanceToGoal(self, kick):
        if self.crossesGoalLine(kick):
            offset = 10000000 # TODO make less of a hack
        else:
            offset = 0

        goalCenter = Location(nogginC.OPP_GOALBOX_RIGHT_X,
                              nogginC.CENTER_FIELD_Y)
        return (offset - math.sqrt((kick.destinationX - goalCenter.x)**2 +
                                   (kick.destinationY - goalCenter.y)**2))
    ###############
    ### FILTERS ###
    ###############

    # can we kick it out of bounds behind our own goal without scoring an own goal?
    def outOfBounds(self, kick):
        bottomGoalline1 = (nogginC.FIELD_WHITE_LEFT_SIDELINE_X, nogginC.GREEN_PAD_X)
        bottomGoalline2 = (nogginC.FIELD_WHITE_LEFT_SIDELINE_X, nogginC.BLUE_GOALBOX_BOTTOM_Y)
        
        topGoalline1 = (nogginC.FIELD_WHITE_LEFT_SIDELINE_X, nogginC.FIELD_WHITE_HEIGHT + nogginC.GREEN_PAD_X)
        topGoalline2 = (nogginC.FIELD_WHITE_LEFT_SIDELINE_X, nogginC.BLUE_GOALBOX_TOP_Y)
        
        k1 = (self.brain.ball.x, self.brain.ball.y)
        k2 = (kick.destinationX, kick.destinationY)

        out = (self.intersects(bottomGoalline1, bottomGoalline2, k1, k2) or 
                        self.intersects(topGoalline1, topGoalline2, k1, k2))
        
        return out

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

        #scale = goalLineX / kickVector[0]
        scale = goalLineX / kick.distance
        # print "Do we cross the goal line: ", scale

        if 0 <= scale <= 1:
            return (rightPostY <= kickVector[1]*scale <= leftPostY)
        return False

    def notTowardOurGoal(self, kick):
        # do not kick into our goalbox
        ball = self.brain.ball

        inBox = (ball.x > nogginC.GREEN_PAD_X and ball.x < nogginC.BLUE_GOALBOX_RIGHT_X and
                    ball.y < nogginC.BLUE_GOALBOX_TOP_Y and ball.y > nogginC.BLUE_GOALBOX_BOTTOM_Y)
        intoBox = (kick.destinationX > nogginC.GREEN_PAD_X and kick.destinationX < nogginC.BLUE_GOALBOX_RIGHT_X and
                    kick.destinationY < nogginC.BLUE_GOALBOX_TOP_Y and kick.destinationY > nogginC.BLUE_GOALBOX_BOTTOM_Y)

        # print "inBox returned"
        if intoBox and not inBox:
            return False

        else:
            goalCenter = Location(nogginC.FIELD_WHITE_LEFT_SIDELINE_X, nogginC.MIDFIELD_Y)
            ball = Location(ball.x, ball.y)
            kickDestination = Location(kick.destinationX, kick.destinationY)

            return goalCenter.distTo(ball) < goalCenter.distTo(kickDestination)


    def forwardProgress(self, kick):
        goalCenter = Location(nogginC.FIELD_WHITE_RIGHT_SIDELINE_X, nogginC.MIDFIELD_Y)
        ball = Location(self.brain.ball.x, self.brain.ball.y)
        kickDestination = Location(kick.destinationX, kick.destinationY)

        return goalCenter.distTo(ball) > goalCenter.distTo(kickDestination)

    def upfieldEnough(self, kick):
        return -40 <= kick.setupH <= 40

    def isShortOrbit(self, kick):
        return math.fabs(self.brain.loc.h - kick.setupH) < 50

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

    ### Functions for determining if two line segments intersect
    # adapted from http://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/ ###

    # Given three colinear points p, q, r, the function checks if
    # point q lies on line segment 'pr'
    def onSegment(self, p, q, r):
        if (q[0] <= max(p[0], r[0]) and q[0] >= min(p[0], r[0]) and
            q[1] <= max(p[1], r[1]) and q[1] >= min(p[1], r[1])):
           return True
     
        return False
     
    # To find orientation of ordered triplet (p, q, r).
    # The function returns following values
    # 0 --> p, q and r are colinear
    # 1 --> Clockwise
    # 2 --> Counterclockwise
    def orientation(self, p, q, r):
        # See 10th slides from following link for derivation of the formula
        # http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf
        val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1])
     
        if val == 0:
            return 0  # colinear
        if val > 0:   # clockwise
            return 1
        return 2      # counter clockwise

    # The main function that returns true if line segment 'p1q1'
    # and 'p2q2' intersect.
    def intersects(self, p1, q1, p2, q2):
        # Find the four orientations needed for general and
        # special cases
        o1 = self.orientation(p1, q1, p2);
        o2 = self.orientation(p1, q1, q2);
        o3 = self.orientation(p2, q2, p1);
        o4 = self.orientation(p2, q2, q1);
     
        # General case
        if (o1 != o2 and o3 != o4):
            return True
     
        # Special Cases
        # p1, q1 and p2 are colinear and p2 lies on segment p1q1
        if (o1 == 0 and self.onSegment(p1, p2, q1)):
            return True
     
        # p1, q1 and p2 are colinear and q2 lies on segment p1q1
        if (o2 == 0 and self.onSegment(p1, q2, q1)):
            return True
     
        # p2, q2 and p1 are colinear and p1 lies on segment p2q2
        if (o3 == 0 and self.onSegment(p2, p1, q2)):
            return True
     
        # p2, q2 and q1 are colinear and q1 lies on segment p2q2
        if (o4 == 0 and self.onSegment(p2, q1, q2)):
            return True
     
        return False # Doesn't fall in any of the above cases
