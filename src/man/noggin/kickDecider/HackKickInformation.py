import KickingConstants as constants
import vision
import kicks

class KickInformation:
    """
    Class to hold all the things we need to decide a kick
    """
    def __init__(self, brain):
#        self.decider = decider
        self.brain = brain

        self.oppGoalLeftPostBearings = []
        self.oppGoalRightPostBearings = []
        self.myGoalLeftPostBearings = []
        self.myGoalRightPostBearings = []

        self.oppGoalLeftPostDists = []
        self.oppGoalRightPostDists = []
        self.myGoalLeftPostDists = []
        self.myGoalRightPostDists = []

        self.oppLeftPostBearing = None
        self.oppRightPostBearing = None
        self.oppAvgPostBearing = None
        self.myLeftPostBearing = None
        self.myRightPostBearing = None
        self.myAvgPostBearing = None

        self.oppLeftPostDist = 0.0
        self.oppRightPostDist = 0.0
        self.oppAvgPostDist = 0.0
        self.myLeftPostDist = 0.0
        self.myRightPostDist = 0.0
        self.myAvgPostDist = 0.0

        self.sawGoal = False
        self.sawOwnGoal = False
        self.sawOppGoal = False

        self.haveData = False

        self.kickObjective = None
        self.kick = None
        self.kickDest = None
        self.destDist = 500.

        self.orbitAngle = 0.0

    def getKickObjective(self):
        """
        Return a kick objective based on what we've observed
        """
        self.calculateDataAverages()

#        if not self.decider.hasKickedOff:
#            self.kickObjective = constants.OBJECTIVE_KICKOFF
#            return self.kickObjective
#        elif self.sawOppGoal:
#            self.kickObjective = constants.OBJECTIVE_SHOOT
#            return self.kickObjective
#        else:
#            self.kickObjective = constants.OBJECTIVE_CLEAR
#            return self.kickObjective

    def collectData(self):
        """
        Collect info on any observed goals
        """
        self.haveData = True

        if self.brain.yglp.vis.on:
            self.sawGoal = True
            if self.brain.yglp.vis.certainty == vision.certainty._SURE:
                self.sawOppGoal = True
                self.oppGoalLeftPostBearings.append(self.brain.oppGoalLeftPost.vis.bearing)
                self.oppGoalLeftPostDists.append(self.brain.oppGoalLeftPost.vis.dist)

        if self.brain.ygrp.vis.on:
            self.sawGoal = True
            if self.brain.ygrp.vis.certainty == vision.certainty._SURE:
                self.sawOppGoal = True
                self.oppGoalRightPostBearings.append(self.brain.oppGoalRightPost.vis.bearing)
                self.oppGoalRightPostDists.append(self.brain.oppGoalRightPost.vis.dist)

    def calculateDataAverages(self):
        """
        calculates averages based on data collected
        """
        if not self.haveData:
            return

        if len(self.oppGoalLeftPostBearings) > 0:
            self.oppLeftPostBearing = (sum(self.oppGoalLeftPostBearings) /
                                       len(self.oppGoalLeftPostBearings))
        if len(self.oppGoalRightPostBearings) > 0:
            self.oppRightPostBearing = (sum(self.oppGoalRightPostBearings) /
                                        len(self.oppGoalRightPostBearings))
        # average post bearings
        if self.oppLeftPostBearing is not None and self.oppRightPostBearing is not None:
            self.oppAvgPostBearing = (self.oppLeftPostBearing + self.oppRightPostBearing)*.5
        elif self.oppLeftPostBearing is not None:
            self.oppAvgPostBearing = self.oppLeftPostBearing + 5. #somewhere in middle
        elif self.oppRightPostBearing is not None:
            self.oppAvgPostBearing = self.oppRightPostBearing - 5. #somewhere in middle

        # distance averages
        if len(self.oppGoalLeftPostDists) > 0:
            self.oppLeftPostDist = (sum(self.oppGoalLeftPostDists) /
                                    len(self.oppGoalLeftPostDists))
        if len(self.oppGoalRightPostDists) > 0:
            self.oppRightPostDist = (sum(self.oppGoalRightPostDists) /
                                     len(self.oppGoalRightPostDists))
        # average post distances
        if self.oppLeftPostBearing is not None and self.oppRightPostBearing is not None:
            self.oppAvgPostDist = (self.oppLeftPostDist + self.oppRightPostDist)*.5
        elif self.oppLeftPostBearing is not None:
            self.oppAvgPostDist = self.oppLeftPostDist
        elif self.oppRightPostBearing is not None:
            self.oppAvgPostDist = self.oppRightPostDist

    def dangerousBall(self):
        for mate in self.brain.teamMembers:
            if mate.playerNumber in [1, 4] and mate.active:
                if mate.ballOn and mate.ballDist < 100:
                    return True
                
        return False


    def shoot(self):
        """
        returns the kick we should do in a shooting situation
        """
        rightPostBearing = self.oppRightPostBearing
        leftPostBearing = self.oppLeftPostBearing

        if self.dangerousBall() and self.oppAvgPostDist < 200:
            self.brain.speech.say("Dangerous ball!")
            return self.chooseBackKick()

        # first determine if both opp goal posts were seen
        if (rightPostBearing is not None and leftPostBearing is not None):
            # if we are facing between the posts
            if (leftPostBearing + constants.KICK_STRAIGHT_POST_BEARING >= 0 and \
                    rightPostBearing - constants.KICK_STRAIGHT_POST_BEARING <= 0):
                
                if not self.sawGoal:
                    return self.chooseBackKick()
                elif self.oppAvgPostDist == 0 or self.dangerousBall():
                    return self.chooseShortKick()
                else:
                    self.brain.speech.say("Take it to the house!")
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

    def chooseShortKick(self):
        ball = self.brain.ball
        if ball.loc.relY > 0:
            return kicks.LEFT_SHORT_STRAIGHT_KICK
        return kicks.RIGHT_SHORT_STRAIGHT_KICK

    def chooseDynamicKick(self):
        ball = self.brain.ball
        if ball.loc.relY > 0:
            return kicks.LEFT_QUICK_STRAIGHT_KICK
        return kicks.RIGHT_QUICK_STRAIGHT_KICK

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

    def chooseBackKick(self):
        """
        Picks the back kick based on which foot is closer to center field.
        This way, we minimize the chance of kicking out of bounds.
        """
        #Disabled for now, need more testing
        """
        if self.brain.my.y > NogginConstants.CENTER_FIELD_Y:
            return kicks.LEFT_SHORT_BACK_KICK
        else:
            return kicks.RIGHT_SHORT_BACK_KICK
        """
        if self.shouldFrontKickLeft():
            return kicks.LEFT_SHORT_BACK_KICK
        else:
            return kicks.RIGHT_SHORT_BACK_KICK

    def shouldFrontKickLeft(self):
        """
        How we choose which kick to do when we're facing the ball. For now,
        simply pick the foot that's closer to the ball.
        """
        if self.brain.ball.loc.relY > 0:
            return True
        else:
            return False

    def __str__(self):
        s = ""
        if self.oppLeftPostBearing is not None:
            s += ("Opp left post bearing is: " + str(self.oppLeftPostBearing) +
                  " dist is: " + str(self.oppLeftPostDist) + "\n")
        if self.oppRightPostBearing is not None:
            s += ("Opp right post bearing is: " + str(self.oppRightPostBearing)
                  + " dist is: " + str(self.oppRightPostDist) +  "\n")
        if s == "":
            s = "No goal posts observed"
#        if self.kickObjective == constants.OBJECTIVE_SHOOT:
#            s += "\nObjective is: SHOOT"
#        if self.kickObjective == constants.OBJECTIVE_CLEAR:
#            s += "\nObjective is: CLEAR"
        return s
