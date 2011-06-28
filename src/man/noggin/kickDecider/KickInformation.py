import KickingConstants as constants
from .. import NogginConstants
from ..typeDefs.Location import Location
"""
Rewritten by Wils Dawson (6/28/11)
"""


class KickInformation:
    """
    Class to hold all the things we need to decide a kick
    """
    def __init__(self, brain):
        self.brain = brain

        self.kickPossibilities = {"scoringKick"   : None,
                                  "passingKick"   : None,
                                  "clearingKick"  : None,
                                 # "AdvancingKick" : None,
                                 # "CrossingKick"  : None,
                                  "PassBackKick"  : None }
        self.passingTeammate = None     # Teammate to pass to.
        self.kickObjective = None
        self.kick = None
        self.kickDest = None # put in kick
        self.destDist = 0.0       # put in kick
        self.orbitAngle = 0.0      # put in kick

    def canScoreAll(self):
        return (self.brain.ball.distTo(self.brain.oppGoalRightPost) <
                constants.SHORT_RANGE_KICK_DIST and
                self.brain.ball.distTo(self.brain.oppGoalLeftPost) <
                constants.SHORT_RANGE_KICK_DIST) # and open field

    def canScoreSome(self):
        return (self.brain.ball.distTo(self.brain.oppGoalRightPost) <
                constants.LONG_RANGE_KICK_DIST or
                self.brain.ball.distTo(self.brain.oppGoalLeftPost) <
                constants.LONG_RANGE_KICK_DIST) # and open field.

    def openTeammateCanScore(self):
        # determine out of which teammates are open and in front of us,
        # if there is one that can score. Set it to PassingTeammate.
        teammates = self.openForwardTeammates()
        bestMate = None
        minDist = NogginConstants.FIELD_WIDTH
        for mate in teammates:
            if (mate.distTo(self.brain.oppGoalRightPost) <
                constants.LONG_RANGE_KICK_DIST or
                mate.distTo(self.brain.oppGoalLeftPost) <
                constants.LONG_RANGE_KICK_DIST):
                # minimize on longest distance to select mate with
                # most scoring options.
                longest = max(mate.distTo(self.brain.oppGoalRightPost),
                              mate.distTo(self.brain.oppGoalLeftPost))
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
            if mate.x > self.brain.my.x:
                forwardMates.append(mate)
        return forwardMates
