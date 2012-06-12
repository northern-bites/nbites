import KickingConstants as constants
import vision
import kicks
from objects import Location
import noggin_constants as nogginConstants

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
        if self.oppLeftPostBearing is not None and \
                self.oppRightPostBearing is not None:
            self.oppAvgPostBearing = (self.oppLeftPostBearing +
                                      self.oppRightPostBearing)*.5
        elif self.oppLeftPostBearing is not None:
            self.oppAvgPostBearing = self.oppLeftPostBearing + 5.#somewhere in middle
        elif self.oppRightPostBearing is not None:
            self.oppAvgPostBearing = self.oppRightPostBearing - 5.#somewhere in middle

        # distance averages
        if len(self.oppGoalLeftPostDists) > 0:
            self.oppLeftPostDist = (sum(self.oppGoalLeftPostDists) /
                                    len(self.oppGoalLeftPostDists))
        if len(self.oppGoalRightPostDists) > 0:
            self.oppRightPostDist = (sum(self.oppGoalRightPostDists) /
                                     len(self.oppGoalRightPostDists))
        # average post distances
        if self.oppLeftPostBearing is not None and \
                self.oppRightPostBearing is not None:
            self.oppAvgPostDist = (self.oppLeftPostDist + self.oppRightPostDist)*.5
        elif self.oppLeftPostBearing is not None:
            self.oppAvgPostDist = self.oppLeftPostDist
        elif self.oppRightPostBearing is not None:
            self.oppAvgPostDist = self.oppRightPostDist

    # Hack from US open 2012
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

        # DEBUG printing
        print "In method shoot."

        # Is loc GOOD_ENOUGH for a kick decision?
        # TODO: make sure this uses orbits
        if False: #self.brain.my.getLocScore() == nogginConstants.GOOD_LOC:
            relLocationBallToGoal = ball.loc.relativeLocationOf(Location(670,270))
            bearingBallToGoal = relLocationBallToGoal.bearing
            # Assume our bearing at the ball will equal our current bearing
            relLocationMeToBall = self.brain.my.relativeLocationOf(ball.loc)
            bearingMeToBall = relLocationMeToBall.bearing

            bearingDifference = bearingBallToGoal - bearingMeToBall

            if bearingDifference < 45 and bearingDifference > -45:
                #choose straight kick!
                return self.chooseDynamicKick()
            elif bearingDifference > 45 and bearingDifference < 135:
                #choose a right side kick! (using right foot)
                return kicks.RIGHT_SIDE_KICK
            elif bearingDifference < -45 and bearingDifference > -135:
                #choose a left side kick! (using left foot)
                return kicks.LEFT_SIDE_KICK
            else:
                #choose a back kick!
                return self.chooseBackKick()


        # Loc is bad- use only visual information to choose a kick.
        #TODO: add orbits to this

        rightPostBearing = self.oppRightPostBearing
        leftPostBearing = self.oppLeftPostBearing

        # DEBUG printing
        print "rightPostBearing: ",rightPostBearing
        print "leftPostBearing:  ",leftPostBearing

        if rightPostBearing is None and leftPostBearing is None:
            # Can't see any posts: orbit.
            return self.kickLoc()
        elif rightPostBearing is not None and leftPostBearing is not None:
            # Can see both posts: shoot between them.
            leftScorePoint = rightPostBearing - 20
            rightScorePoint = leftPostBearing + 20

            if leftScorePoint < rightScorePoint:
                # less than 40 degrees of goal available to shoot in.
                leftScorePoint = ((rightPostBearing + leftPostBearing) / 2) + 5
                rightScorePoint = leftScorePoint - 10
        elif rightPostBearing is not None:
            # Can only see the right post.
            leftScorePoint = rightPostBearing - 15
            rightScorePoint = leftScorePoint - 10
        else:
            # Can only see the left post.
            rightScorePoint = leftPostBearing + 15
            leftScorePoint = rightScorePoint +10

        # DEBUG printing
        print "rightScorePoint: ",rightScorePoint
        print "leftScorePoint:  ",leftScorePoint

        # If any kick is currently valid, choose that kick.
        if leftScorePoint > 0 and rightScorePoint < 0:
            return self.chooseDynamicKick()
        elif leftScorePoint > 90 and rightScorePoint < 90:
            return kicks.RIGHT_SIDE_KICK
        elif leftScorePoint > -90 and rightScorePoint < -90:
            return kicks.LEFT_SIDE_KICK
        elif leftScorePoint < -90 and rightScorePoint > 90:
            return self.chooseBackKick()

        # Choose whichever kick is closest to being between the score points.
        # Note: no kick bearing is between the posts, so they are all
        #   to the right of the rightScorePoint or left of leftScorePoint.
        if rightScorePoint > 90:
                # Quadrent 2
            if (180 - leftScorePoint) - (rightScorePoint - 90) < 0:
                    #Closer to the leftScorePoint
                return self.chooseBackKick()
            else:
                return kicks.RIGHT_SIDE_KICK
        elif rightScorePoint > 0:
            # Quadrent 1
            if (90 - leftScorePoint) - (rightScorePoint - 0) < 0:
                return kicks.RIGHT_SIDE_KICK
            else:
                return self.chooseDynamicKick()
        elif rightScorePoint > -90:
            # Quadrent 4
            if (0 - leftScorePoint) - (rightScorePoint + 90) < 0:
                return self.chooseDynamicKick()
            else:
                return kicks.LEFT_SIDE_KICK
        else:
            # Quadrent 3
            if (-90 - leftScorePoint) - (rightScorePoint + 180) < 0:
                return kicks.LEFT_SIDE_KICK
            else:
                return self.chooseBackKick()

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
            return kicks.LEFT_STRAIGHT_KICK
        return kicks.RIGHT_STRAIGHT_KICK

    def kickLoc(self):
        """
        returns kick using localization
        """

        # DEBUG printing
        print "In method kickLoc."

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
