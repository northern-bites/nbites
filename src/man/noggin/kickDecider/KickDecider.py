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
        self.hasKickedOff = True

        self.info = KickInformation.KickInformation(self, brain)

    def resetInfo(self):
        """
        resets kickInfo so we can decide on next kick
        """
        self.info = KickInformation.KickInformation(self, self.brain)

    def collectInfo(self):
        """
        calls kickInfo to collect data
        """
        self.info.collectData()

    def getKick(self):
        """
        returns the kick and decides on one if we haven't picked one yet
        """
        if self.info.kick is None:
            self.info.kick = self.decideKick()
        return self.info.kick

    def getSweetMove(self):
        """
        returns the sweet move required for motion to kick
        """
        currentKick = self.getKick()
        if currentKick == kicks.LEFT_DYNAMIC_STRAIGHT_KICK or \
                currentKick == kicks.RIGHT_DYNAMIC_STRAIGHT_KICK:
            ball = self.brain.ball
            dist = self.info.destDist
            return currentKick.sweetMove(ball.relY, dist)
        else:
            return currentKick.sweetMove

    def getKickObjective(self):
        self.info.getKickObjective()

    def decideKick(self):
        """
        using objective and heuristics and localization determines best kick
        In April 2011, Localization is too unreliable. We use it as a last resort
        """
        self.getKickObjective()

        print self.info

        if self.info.kickObjective == constants.OBJECTIVE_KICKOFF:
            return self.kickOff() #should never happen
        elif self.info.kickObjective == constants.OBJECTIVE_SHOOT:
            return self.shoot()
        #elif self.info.kickObjective == constants.OBJECTIVE_CLEAR:
        else:
            return self.clear()

    def kickOff(self):
        """
        returns the kick we should do in the kickOff situation
        NOT TO BE USED!!! Handled elsewhere.
        """
        self.hasKickedOff = True
        if self.brain.playbook.pb.kickoffFormation == 0:
            return kicks.RIGHT_SIDE_KICK
        else:
            return kicks.LEFT_SIDE_KICK

    def shoot(self):
        """
        returns the kick we should do in a shooting situation
        """
        rightPostBearing = self.info.oppRightPostBearing
        leftPostBearing = self.info.oppLeftPostBearing

        # first determine if both opp goal posts were seen
        if (rightPostBearing is not None and leftPostBearing is not None):
            # if we are facing between the posts
            if (leftPostBearing + constants.KICK_STRAIGHT_POST_BEARING >= 0 and \
                    rightPostBearing - constants.KICK_STRAIGHT_POST_BEARING <= 0):
                return self.chooseDynamicKick()
            # if the goal is to our right, use our left foot
            elif leftPostBearing < 0:
                return kicks.LEFT_SIDE_KICK
            # if the goal is to our left, use our right foot
            elif rightPostBearing > 0:
                return kicks.RIGHT_SIDE_KICK
        # if only one was seen
        elif (rightPostBearing is not None):
            # if the right post is roughly to our right (but not too far), kick straight
            if (rightPostBearing - constants.KICK_STRAIGHT_POST_BEARING <= 0 and \
                    rightPostBearing >= -1*constants.KICK_STRAIGHT_BEARING_THRESH):
                return self.chooseDynamicKick()
            # if the right post is roughly to our left, kick right
            elif (rightPostBearing > 0):
                return kicks.RIGHT_SIDE_KICK
            # if the right post is way to our right, kick with the left foot
            elif (rightPostBearing < -1*constants.KICK_STRAIGHT_BEARING_THRESH):
                return kicks.LEFT_SIDE_KICK
        elif (leftPostBearing is not None):
            if (leftPostBearing + constants.KICK_STRAIGHT_POST_BEARING >= 0 and \
                    leftPostBearing <= constants.KICK_STRAIGHT_BEARING_THRESH):
                return self.chooseDynamicKick()
            elif (leftPostBearing < 0):
                return kicks.LEFT_SIDE_KICK
            elif (leftPostBearing > constants.KICK_STRAIGHT_BEARING_THRESH):
                return kicks.RIGHT_SIDE_KICK
        # if none were seen
        return self.kickLoc()

    def clear(self):
        """
        returns kick we should do in a clearing situation
        """
        rightPostBearing = self.info.myRightPostBearing
        rightPostDist = self.info.myRightPostDist
        leftPostBearing = self.info.myLeftPostBearing
        leftPostDist = self.info.myLeftPostDist

        # first determine if both my goal posts were seen
        if (rightPostBearing is not None and leftPostBearing is not None):
            distDiff = rightPostDist - leftPostDist
            # if we are facing between our posts and difference between dists is small enough
            if (rightPostBearing >= 0 and leftPostBearing <= 0 and \
                    distDiff <= constants.CLEAR_POST_DIST_DIFF):
                return self.chooseBackKick()
            elif (rightPostDist <= leftPostDist):
                return kicks.LEFT_SIDE_KICK
            elif (leftPostDist < rightPostDist):
                return kicks.RIGHT_SIDE_KICK
        # if only one was seen
        elif (rightPostBearing is not None):
            if (rightPostBearing > 0):
                return kicks.LEFT_SIDE_KICK
            else:
                return kicks.RIGHT_SIDE_KICK
        elif (leftPostBearing is not None):
            if (leftPostBearing > 0):
                return kicks.LEFT_SIDE_KICK
            else:
                return kicks.RIGHT_SIDE_KICK
        return self.kickLoc()

    def kickLoc(self):
        """
        returns kick using localization
        """
        my = self.brain.my
        if (my.h <= 45. and my.h >= -45.):
            return self.chooseDynamicKick()
        elif (my.h <= 135. and my.h > 45.):
            return kicks.LEFT_SIDE_KICK
        elif (my.h >= -135. and my.h < -45.):
            return kicks.RIGHT_SIDE_KICK
        else:
            return self.chooseBackKick()

    def chooseDynamicKick(self):
        ball = self.brain.ball
        if ball.relY > 0:
            return kicks.LEFT_DYNAMIC_STRAIGHT_KICK
        return kicks.RIGHT_DYNAMIC_STRAIGHT_KICK

    def chooseBackKick(self):
        ball = self.brain.ball
        if ball.relY > 0:
            return kicks.LEFT_BACK_KICK
        return kicks.RIGHT_BACK_KICK
