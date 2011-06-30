import kicks
import KickInformation
import KickingConstants as constants
from .. import NogginConstants

class KickDecider(object):
    """
    Uses current info gathered by KickInformation to determine the
    best possible kick and where we should position when called
    """

    def __init__(self, brain):
        self.brain = brain
        self.info = KickInformation.KickInformation(brain)

    def resetInfo(self):
        """
        resets kickInfo so we can decide on next kick
        """
        self.info = KickInformation.KickInformation(self, self.brain)

    def getKick(self):
        """
        returns the kick we have decided. If None, then orbit for Loc.
        """
        return self.info.kick

    def setKick(self, k):
        """
        sets a particular kick
        """
        self.info.kick = k

    def getSweetMove(self):
        """
        returns the sweet move required for motion to kick
        """
        kick = self.info.kick
        # TODO make this check unneccessary by making all kicks dynamic.
        if kick == kicks.LEFT_DYNAMIC_STRAIGHT_KICK or \
                kick == kicks.RIGHT_DYNAMIC_STRAIGHT_KICK:
            ball = self.brain.ball
            dist = self.info.destDist
            return kick.sweetMove(ball.relY, dist)
        else:
            return kick.sweetMove

    def getKickObjective(self):
        self.info.kickObjective

    def setKickOff(self):
        """
        sets the kick we should do in the kickOff situation
        """
        smallTeam = self.brain.playbook.pb.numActiveFieldPlayers < 3

        # if there are too few players on the field to do a side kick pass.
        if smallTeam:
            print "Kickoff!"
            self.setKick(self.chooseShortQuickKick())
            self.info.destDist = 100.
        # do a side kick pass depending on where the offender is.
        elif self.brain.playbook.pb.kickoffFormation == 0:
            self.setKick(kicks.RIGHT_SIDE_KICK)
            print "Kickoff RIGHT_SIDE_KICK"
        else:
            self.setKick(kicks.LEFT_SIDE_KICK)
            print "Kickoff LEFT_SIDE_KICK"

        self.info.kickObjective = constants.OBJECTIVE_KICKOFF

    def decideKick(self):
        """
        using objective and heuristics and localization determines best kick
        """
        # Check localization to make sure it's good enough.
        if self.brain.my.locScore != NogginConstants.GOOD_LOC:
            self.info.kick = None
            return

        if self.info.canScoreAll():
            self.score()
        elif self.info.canScoreSome():
            self.chooseScoringKick()
        if self.info.openTeammateCanScore():
            self.chooseOneTimerKick()
        elif self.info.openTeammate():
            self.choosePassingKick()
        if self.info.canClear():
            self.chooseClearingKick()
        if self.info.canPassBack():
            self.choosePassBackKick()

        self.info.kick = self.chooseKick()

    def score(self):
        """
        We are confident we can score with any kick.
        """

        # First we want to figure out which aim point is better.
        # For now just worry about which is faster to kick for,
        # Don't worry about open field yet (6/28/11)

        kickDest = self.info.bestAlignedDest([constants.SHOOT_RIGHT_AIM_POINT,
                                              constants.SHOOT_LEFT_AIM_POINT])

        # Next we want to find the best kick that will hit that point.
        # Since we know any kick will score, we don't have to worry about
        # any range determinations.

        kickDirection = self.info.bestAlignedKickDirection(kickDest)

        if kickDirection == 0:
            #STRAIGHT_KICK
            return
        elif kickDirection == 90:
            #RIGHT_SIDE_KICK
            return
        elif kickDirection == -90:
            #LEFT_SIDE_KICK
            return
        else:
            #BACK_KICK
            return


        """
        # Note: may want to use headingTo(yglp) etc...
        oppLeftPost = self.brain.oppGoalLeftPost
        oppRightPost = self.brain.oppGoalRightPost

        if (my.headingTo(oppLeftPost, forceCalc = True) > my.h >
            my.headingTo(oppRightPost, forceCalc = True)):
            return self.chooseDynamicKick()
        elif (my.headingTo(oppLeftPost, forceCalc = True) > -1*my.h >
              my.headingTo(oppRightPost, forceCalc = True)):
            return self.chooseShortBackKick()
        elif (my.h > 0):
            print "LEFT_SIDE"
            return kicks.LEFT_SIDE_KICK
        else:
            print "RIGHT_SIDE"
            return kicks.RIGHT_SIDE_KICK
        """
