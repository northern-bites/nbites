import kicks
import KickingConstants as constants
from .. import NogginConstants
from ..util import MyMath
from math import fabs
"""
Rewritten by Wils Dawson (6/28/11)
"""

class KickInformation:
    """
    Class to hold all the things we need to decide a kick
    """
    def __init__(self, brain):
        self.brain = brain

        # Stores the kicks in a dictionary before setting the kick field.
        self.kickChoices = {'scoringKick'   : None,
                            'passingKick'   : None,
                            'clearingKick'  : None,
                            # 'AdvancingKick' : None,
                            # 'CrossingKick'  : None,
                            'PassBackKick'  : None, }
        self.passingTeammate = None     # Teammate to pass to.
        self.kickObjective = None
        self.kick = None

    def canScoreAll(self):
        return (self.brain.ball.distTo(constants.SHOOT_LEFT_AIM_POINT) <
                constants.SHORT_RANGE_KICK_DIST and
                self.brain.ball.distTo(constants.SHOOT_RIGHT_AIM_POINT) <
                constants.SHORT_RANGE_KICK_DIST) # and open field

    def canScoreSome(self):
        return (self.brain.ball.distTo(constants.SHOOT_LEFT_AIM_POINT) <
                constants.LONG_RANGE_KICK_DIST or
                self.brain.ball.distTo(constants.SHOOT_RIGHT_AIM_POINT) <
                constants.LONG_RANGE_KICK_DIST) # and open field.

    def openTeammateCanScore(self):
        # determine out of which teammates are open and in front of us,
        # if there is one that can score. Set it to PassingTeammate.
        teammates = self.openForwardTeammates()
        bestMate = None
        minDist = NogginConstants.FIELD_WIDTH
        for mate in teammates:
            if (mate.distTo(constants.SHOOT_LEFT_AIM_POINT) <
                constants.LONG_RANGE_KICK_DIST or
                mate.distTo(constants.SHOOT_RIGHT_AIM_POINT) <
                constants.LONG_RANGE_KICK_DIST):
                # minimize on longest distance to select mate with
                # most scoring options.
                longest = max(mate.distTo(constants.SHOOT_LEFT_AIM_POINT),
                              mate.distTo(constants.SHOOT_RIGHT_AIM_POINT))
                if longest < minDist:
                    minDist = longest
                    bestMate = mate
        if bestMate != None:
            self.passingTeammate = bestMate
            return True
        return False

    def openTeammate(self):
        # determine out of which teammates are open and in front of us, is best.
        teammates = self.openForwardTeammates()
        bestMate = None
        minDist = NogginConstants.FIELD_WIDTH
        for mate in teammates:
            if mate.distTo(constants.CENTER_BALL_POINT) < minDist:
                minDist = mate.distTo(constants.CENTER_BALL_POINT)
                bestMate = mate
        if bestMate != None:
            self.passingTeammate = bestMate
            return True
        return False

    def canClear(self):
        """
        use instead of advance and cross. simpler. let chooseClearingKick do the work
        """
        # don't use open field for now (6/28/11)
        return True

    def canAdvance(self):
        """
        Make sure the possibility of kicking out of bounds is small
        and that there is open field in front of us.
        """
        # don't use open field for now (6/28/11)
        return (self.brain.ball.x < NogginConstants.LANDMARK_OPP_FIELD_CROSS[0] and
                self.brain.ball.y < (NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y -
                                     constants.BALL_NEAR_LINE_THRESH) and
                self.brain.ball.y > (NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
                                     constants.BALL_NEAR_LINE_THRESH))

    def canCross(self):
        """
        Make sure we can cross and there is open field for us to cross.
        """
        # don't use open field for now (6/28/11)
        return True

    def canPassBack(self):
        # don't use this for now (6/28/11)
        return False

    def openForwardTeammates(self):
        """
        Returns a list of open teammates that are farther down the field than us.
        """
        # currently returns all teammates father down the field, since open field
        # calculations are harder --Wils (6/28/11)
        forwardMates = []
        for mate in self.brain.teamMembers:
            # TODO: make sure mate is in our range.
            if mate.x > self.brain.my.x:
                forwardMates.append(mate)
        return forwardMates

    def bestAlignedDest(self, dests):
        """
        We get our relative heading to the ball based on a 0 heading to each
        of the aim points. We adjust these values to put them in a range from
        0 to 45 degrees, with 45 being aligned just right with the ball to
        execute one of our kicks. We find out which one later, we just want
        to know where to shoot. This conversion lets us decide which aim point
        we are better lined up to shoot for easily (ie with a single compare)
        """

        my = self.brain.my
        ball = self.brain.ball
        bestHeading = -1.
        bestDest = None

        for dest in dests:
            bestHeading = max((fabs(((ball.headingTo(dest) -
                                     my.headingTo(ball))
                                    % 90) - 45)), bestHeading)
            bestDest = dest
        return bestDest

    def bestAlignedKick(self, dest):
        """
        Assumes all kicks can get to the specified param 'dest'. Chooses
        Best kick based on alignment (i.e. least cost in real time).
        We rotate our reference frame so that the ball's heading to the
        dest is 0 degrees and we use our heading to the ball as the best
        guess of what our orientation will be once we get to the ball. Using
        the ball line to the dest as the 0 degree line, we can use the rotated
        heading to let us choose the best kick based on alignment.
        """
        my = self.brain.my
        ball = self.brain.ball
        rotatedHeading = ball.headingTo(dest) - my.headingTo(ball)
        kick = None

        if (fabs(rotatedHeading) < constants.STRAIGHT_KICK_ALIGNMENT_BEARING):
            kick = self.chooseStraightKick()
            kick.heading = ball.headingTo(dest)
        elif (fabs(rotatedHeading) < constants.BACK_KICK_ALIGNMENT_BEARING):
            if MyMath.sign(rotatedHeading) == -1:
                kick = kicks.LEFT_SIDE_KICK
                kick.heading = MyMath.sub180Angle(ball.headingTo(dest) + 90)
            else:
                kick = kicks.RIGHT_SIDE_KICK
                kick.heading = MyMath.sub180Angle(ball.headingTo(dest) - 90)
        else:
            kick = self.chooseBackKick()
            kick.heading = MyMath.sub180Angle(ball.headingTo(dest) + 180)

        kick.dest = dest
        return kick




    def chooseStraightKick(self):
        """
        Picks the straight kick based on our heading to the ball.
        """
        if self.brain.my.headingTo(self.brain.ball) > 0:
            return kicks.RIGHT_DYNAMIC_STRAIGHT_KICK
        else:
            return kicks.LEFT_DYNAMIC_STRAIGHT_KICK

    def chooseBackKick(self):
        """
        Picks the back kick based on which foot is closer to center field.
        This way, we minimize the chance of kicking out of bounds.
        """
        if self.brain.my.y > NogginConstants.CENTER_FIELD_Y:
            return kicks.LEFT_SHORT_BACK_KICK
        else:
            return kicks.RIGHT_SHORT_BACK_KICK
