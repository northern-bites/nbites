import kicks
import KickingConstants2 as constants
import noggin_constants as nogginC
import itertools

# TODO write unit test
# TODO clean everything up, name things better, kicks.py, constants, etc.
# TODO document everything
# TODO integrate with approoach ball
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
        self.possibleKicks = generateNothing()
    
    ### PLANNERS ###
    def pBrunswick(self):
        clearPossibleKicks()
        addShotsOnGoal()
        addPassesToFieldCross()
        addCrosses()
        addClears()

        goodKicks = (kick for kick in self.possibleKicks if closeEnough(kick) and
                                                            inBounds(kick) and
                                                            forwardProgress(kick))

        try:
            return goodKicks.next():
        except StopIteration:
            return False

    ### API ###
    def addShotsOnGoal():
        x = self.brain.ball.x - self.brain.loc.x
        y = self.brain.ball.y - self.brain.loc.y
        theta = math.pi - math.atan(x/y)
        precision = constants.SHOT_PRECISION*math.cos(theta)
        if precision >= constants.MIN_PRECISION:
            self.possibleKicks = itertools.chain(possibleKicks,
                                                 generateKicksFromGoalDest(nogginC.OPP_GOALBOX_RIGHT_X,
                                                                           nogginC.OPP_GOALBOX_MIDDLE_Y,
                                                                           precision))

    def addPassesToFieldCross():
        betweenFieldCrossAndGoalBoxLeft = (nogginC.LANDMARK_OPP_FIELD_CROSS[0] +
                                           nogginC.OPP_GOALBOX_LEFT_X) / 2.
        self.possibleKicks = itertools.chain(possibleKicks,
                                             generateKicksFromGoalDest(betweenFieldCrossAndGoalBoxLeft,
                                                                       nogginC.LANDMARK_OPP_FIELD_CROSS[1],
                                                                       constants.PASS_PRECISION))

    def addCrosses():
        if player.brain.loc.y > nogginC.MIDFIELD_Y:
            post = nogginC.LANDMARK_OPP_GOAL_RIGHT_POST_Y
        else:
            post = nogginC.LANDMARK_OPP_GOAL_LEFT_POST_Y

        self.possibleKicks = itertools.chain(possibleKicks,
                                             generateKicksFromGoalDest(nogginC.LANDMARK_OPP_FIELD_CROSS[0],
                                                                       post
                                                                       costants.PASS_PRECISION))

    def addClears():
        self.possibleKicks = itertools.chain(possibleKicks,
                                             generateFastestPossibleKicks())


    def clearPossibleKicks():
        self.possibleKicks = generateNothing()

    ### GENERATORS ###
    def generateNothing():
        return
        yield

    def generateIdealKicks(idealX, idealY, actualX, actualY):
        for kick in self.kicks:
            # Two transformations based on similar triangles
            x_off = kick.setup.x
            y_off = kick.setup.y
            x1 = actualX - self.brain.ball.x
            y1 = actualY - self.brain.ball.y
            y2 = math.sqrt(x_off**2/(1+(x1**2/y1**2)))
            x2 = x1*y2/y1
            x3 = math.sqrt(y_off**2/(1+(x2**2/y2**2)))
            y3 = x2*x3/y2
            kick.setup.x = x3
            kick.setup.y = y3
            kick.setup.h = math.degrees(math.atan(self.brain.ball.y-y3,
                                                  self.brain.ball.x-x3))
            if self.brain.ball.x < x3: kick.heading += 180
            kick.destination.x = idealX
            kick.destination.y = idealY
            yield kick

    def generateFastestPossibleKicks():
        fastestKickXDest = 2.*self.brain.ball.x - self.brain.loc.x
        fastestKickYDest = 2.*self.brain.ball.y - self.brain.loc.y
        yield generateIdealKicks(fastestKickXDest,fastestKickYDest,
                                 fastestKickXDest,fastestKickYDest)

    # NOTE N should be an odd number, so that a perfectly-aimed kick is chosen
    #      as one of the sampled kicks
    def sampleKicks(x, y, precision, N):
        r, theta_inital = fromCartesianToPolarCoordinates(x,y)
        big_theta = 2*math.atan(precision/r) # TODO prec or 2*prec?
        little_theta = big_theta/N
        theta = theta_initial - big_theta/2.
        for i in xrange(N):
            new_x, new_y = fromPolarToCartesianCoordinates(r,theta)
            yield generateIdealKicks(x,y,new_x,new_y)
            theta += little_theta

    def generateKicksFromGoalDest(x, y, precision):
        possibleKicks = sampleKicks(x,y,precision,constants.NUM_OF_SAMPLES)

        def scoreKick(kick):
            accuracy = scoreAccuracy(calculateKickErrorInY(kick))
            distance = scoreDistance(self.brain.loc.distTo(kick.getPosition()))
            return constants.GAMMA*accuracy + constants.DELTA*distance

        yield max(possibleKicks,key=scoreKick)

    ### FILTERS ###
    def closeEnough(kick):
        if kick.destination:
            return kick.destination.distTo(self.brain.ball) < kick.maxDist
        return True

    # TODO ask a path planner to do this for us?
    #      is this necessary? already optimized against euclid distance...
    def fastEnough(kick):
        pass

    def inBounds(kick):
        return (kick.destination.x >= FIELD_WHITE_LEFT_SIDELINE_X and 
                kick.destination.x <= FIELD_WHITE_LEFT_SIDELINE_Y and
                kick.destination.y >= 0. and
                kick.destination.y <= 2.*CENTER_FIELD_Y) # TODO hack

    # TODO is this necessary?
    def forwardProgress(kick):
        return (kick.destination.x - self.brain.ball.x > 
                constants.FORWARD_ENOUGH_THRES)

    ### HELPER FUNCTIONS ###
    def calculateKickErrorInY(kick):
        theta = 180. - kick.setup.headingTo(kick.destination)
        return self.brain.ball.distTo(kick.destination)*math.sin(math.radians(theta))

    def scoreAccuracy(x):
        return math.e**(-constants.ALPHA*x**2)

    def scoreDistance(x):
        return math.e**(-constants.BETA*x**2)

    def fromCartesianToPolarCoordinates(x, y):
        return math.sqrt(x**2+y**2), math.atan2(y,x)

    def fromPolarToCartesianCoordinates(r, theta):
        return r*math.cos(theta), r*math.sin(theta)
