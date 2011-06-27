import kicks
import KickInformation
import KickingConstants as constants

class KickDecider(object):
    """
    Uses current info gathered by KickInformation to determine the
    best possible kick and where we should position when called
    """

    def __init__(self, brain):
        self.brain = brain

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

    def setKick(self, k):
        """
        sets a particular kick
        """
        self.info.kick = k

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

    def getCenterKickPosition(self):
        return kicks.CENTER_KICK_POSITION

    def decideKick(self):
        """
        using objective and heuristics and localization determines best kick
        In April 2011, Localization is too unreliable. We use it as a last resort
        """
        self.getKickObjective()

        print self.info

        if self.info.kickObjective == constants.OBJECTIVE_SHOOT:
            return self.shoot()
        #elif self.info.kickObjective == constants.OBJECTIVE_CLEAR:
        else:
            return self.clear()

    def setKickOff(self):
        """
        sets the kick we should do in the kickOff situation
        """
        smallTeam = self.brain.playbook.pb.numActiveFieldPlayers < 3

        # if there are too few players on the field to do a side kick pass.
        if smallTeam:
            if self.brain.ball.relY >= 0:
                self.setKick(kicks.LEFT_DYNAMIC_STRAIGHT_KICK)
                print "Kickoff STRAIGHT_LEFT_KICK"
            else:
                self.setKick(kicks.RIGHT_DYNAMIC_STRAIGHT_KICK)
                print "Kickoff STRAIGHT_RIGHT_KICK"
            self.info.destDist = 100.
        # do a side kick pass depending on where the offender is.
        elif self.brain.playbook.pb.kickoffFormation == 0:
            self.setKick(kicks.RIGHT_SIDE_KICK)
            print "Kickoff RIGHT_SIDE_KICK"
        else:
            self.setKick(kicks.LEFT_SIDE_KICK)
            print "Kickoff LEFT_SIDE_KICK"

        self.info.kickObjective = constants.OBJECTIVE_KICKOFF

    def shoot(self):
        """
        returns the kick we should do in a shooting situation
        """
        rightPostBearing = self.info.oppRightPostBearing
        rightPostDist = self.info.oppRightPostDist
        leftPostBearing = self.info.oppLeftPostBearing
        leftPostDist = self.info.oppLeftPostDist

        # first determine if both opp goal posts were seen
        if (rightPostBearing is not None and leftPostBearing is not None):
            # if we are facing between the posts
            if (leftPostBearing + constants.KICK_STRAIGHT_POST_BEARING >= 0 and
                rightPostBearing - constants.KICK_STRAIGHT_POST_BEARING <= 0):
                return self.chooseDynamicKick()
            # if the goal is to our right, use our left foot
            elif leftPostBearing < 0:
                print "LEFT_SIDE"
                return kicks.LEFT_SIDE_KICK
            # if the goal is to our left, use our right foot
            elif rightPostBearing > 0:
                print "RIGHT_SIDE"
                return kicks.RIGHT_SIDE_KICK
        # if only one was seen
        elif (rightPostBearing is not None):
            # if the right post is roughly to our right (but not too far),
            # and it's not really close to us. kick straight
            if (rightPostBearing - constants.KICK_STRAIGHT_POST_BEARING <= 0 and
                rightPostBearing >= -1*constants.KICK_STRAIGHT_BEARING_THRESH and
                rightPostDist > constants.KICK_SIDE_DIST_THRESH):
                return self.chooseDynamicKick()
            # if the right post is roughly to our left, kick right
            elif (rightPostBearing > 0):
                print "RIGHT_SIDE"
                return kicks.RIGHT_SIDE_KICK
            # if the right post is way to our right, kick with the left foot
            elif (-1*constants.KICK_STRAIGHT_BEARING_THRESH > rightPostBearing):
                print "LEFT_SIDE"
                return kicks.LEFT_SIDE_KICK
        elif (leftPostBearing is not None):
            if (leftPostBearing + constants.KICK_STRAIGHT_POST_BEARING >= 0 and
                leftPostBearing <= constants.KICK_STRAIGHT_BEARING_THRESH and
                leftPostDist > constants.KICK_SIDE_DIST_THRESH):
                return self.chooseDynamicKick()
            elif (0 > leftPostBearing):
                print "LEFT_SIDE"
                return kicks.LEFT_SIDE_KICK
            elif (leftPostBearing > constants.KICK_STRAIGHT_BEARING_THRESH):
                print "RIGHT_SIDE"
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
            # if we are facing between our posts and
            # difference between dists is small enough
            if (rightPostBearing >= 0 and leftPostBearing <= 0 and
                distDiff <= constants.CLEAR_POST_DIST_DIFF):
                return self.chooseLongBackKick()
            elif (rightPostDist <= leftPostDist):
                print "LEFT_SIDE"
                return kicks.LEFT_SIDE_KICK
            elif (leftPostDist < rightPostDist):
                print "RIGHT_SIDE"
                return kicks.RIGHT_SIDE_KICK
        # if only one was seen
        elif (rightPostBearing is not None):
            if (rightPostBearing > 0):
                print "LEFT_SIDE"
                return kicks.LEFT_SIDE_KICK
            else:
                print "RIGHT_SIDE"
                return kicks.RIGHT_SIDE_KICK
        elif (leftPostBearing is not None):
            if (leftPostBearing > 0):
                print "LEFT_SIDE"
                return kicks.LEFT_SIDE_KICK
            else:
                print "RIGHT_SIDE"
                return kicks.RIGHT_SIDE_KICK
        return self.kickLoc()

    def kickLoc(self):
        """
        returns kick using localization
        """
        my = self.brain.my
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

        if (my.h <= 20. and my.h >= -20.):
            return self.chooseDynamicKick()
        elif (my.h <= 160. and my.h > 20.):
            print "LEFT_SIDE"
            return kicks.LEFT_SIDE_KICK
        elif (my.h >= -160. and my.h < -20.):
            print "RIGHT_SIDE"
            return kicks.RIGHT_SIDE_KICK
        else:
            return self.chooseShortBackKick()

    def chooseDynamicKick(self):
        ball = self.brain.ball
        if ball.relY >= 0:
            print "LEFT_DYNAMIC_STRAIGHT"
            return kicks.LEFT_DYNAMIC_STRAIGHT_KICK
        print "RIGHT_DYNAMIC_STRAIGHT"
        return kicks.RIGHT_DYNAMIC_STRAIGHT_KICK

    def chooseLongBackKick(self):
        ball = self.brain.ball
        if ball.relY > 0:
            print "LEFT_LONG_BACK"
            return kicks.LEFT_LONG_BACK_KICK
        print "RIGHT_LONG_BACK"
        return kicks.RIGHT_LONG_BACK_KICK

    def chooseShortBackKick(self):
        ball = self.brain.ball
        if ball.relY > 0:
            print "LEFT_SHORT_BACK"
            return kicks.LEFT_SHORT_BACK_KICK
        print "RIGHT_SHORT_BACK"
        return kicks.RIGHT_SHORT_BACK_KICK

    def chooseShortQuickKick(self):
        ball = self.brain.ball
        if ball.relY > 0:
            print "SHORT_QUICK_LEFT"
            return kicks.SHORT_QUICK_LEFT_KICK
        print "SHORT_QUICK_RIGHT"
        return kicks.SHORT_QUICK_RIGHT_KICK
