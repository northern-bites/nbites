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

        self.farGoalLeftPostBearings = []
        self.farGoalRightPostBearings = []
        self.nearGoalLeftPostBearings = []
        self.nearGoalRightPostBearings = []

        self.farGoalLeftPostDists = []
        self.farGoalRightPostDists = []
        self.nearGoalLeftPostDists = []
        self.nearGoalRightPostDists = []

        self.farLeftPostBearing = 0.0
        self.farRightPostBearing = 0.0
        self.nearLeftPostBearing = 0.0
        self.nearRightPostBearing = 0.0

        self.farLeftPostDist = 0.0
        self.farRightPostDist = 0.0
        self.farAvgPostDist = 0.0
        self.nearLeftPostDist = 0.0
        self.nearRightPostDist = 0.0
        self.nearAvgPostDist = 0.0

        self.sawGoal = False
        self.sawNearGoal = False
        self.sawFarGoal = False

        self.haveData = False

        self.aimAtOtherGoal = False
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
                if brain.yglp.vis.dist > 300:
                    self.farGoalLeftPostBearings.append(self.brain.yglp.vis.bearing)
                    self.farGoalLeftPostDists.append(self.brain.yglp.vis.dist)
                if brain.yglp.vis.dist < 300:
                    self.nearGoalLeftPostBearings.append(self.brain.yglp.vis.bearing)
                    self.nearGoalLeftPostDists.append(self.brain.yglp.vis.dist)

        if self.brain.ygrp.vis.on:
            self.sawGoal = True
            if self.brain.ygrp.vis.certainty == vision.certainty._SURE:
                if brain.ygrp.vis.dist > 300:
                    self.farGoalRightPostBearings.append(self.brain.ygrp.vis.bearing)
                    self.farGoalRightPostDists.append(self.brain.ygrp.vis.dist)
                if brain.ygrp.vis.dist < 300:
                    self.nearGoalRightPostBearings.append(self.brain.ygrp.vis.bearing)
                    self.nearGoalRightPostDists.append(self.brain.ygrp.vis.dist)

    def calculateDataAverages(self):
        """
        calculates averages based on data collected
        """
        if not self.haveData:
            return

        # bearing averages
        if len(self.farGoalLeftPostBearings) > 0:
            self.farLeftPostBearing = (sum(self.farGoalLeftPostBearings) /
                                       len(self.farGoalLeftPostBearings))
        if len(self.farGoalRightPostBearings) > 0:
            self.farRightPostBearing = (sum(self.farGoalRightPostBearings) /
                                        len(self.farGoalRightPostBearings))
        if len(self.nearGoalLeftPostBearings) > 0:
            self.nearLeftPostBearing = (sum(self.nearGoalLeftPostBearings) /
                                        len(self.nearGoalLeftPostBearings))
        if len(self.nearGoalRightPostBearings) > 0:
            self.nearRightPostBearing = (sum(self.nearGoalRightPostBearings) /
                                         len(self.nearGoalRightPostBearings))
        # distance averages
        if len(self.farGoalLeftPostDists) > 0:
            self.farLeftPostDist = (sum(self.farGoalLeftPostDists) /
                                    len(self.farGoalLeftPostDists))
        if len(self.farGoalRightPostDists) > 0:
            self.farRightPostDist = (sum(self.farGoalRightPostDists) /
                                     len(self.farGoalRightPostDists))
        if len(self.nearGoalLeftPostDists) > 0:
            self.nearLeftPostDist = (sum(self.nearGoalLeftPostDists) /
                                     len(self.nearGoalLeftPostDists))
        if len(self.nearGoalRightPostDists) > 0:
            self.nearRightPostDist = (sum(self.nearGoalRightPostDists) /
                                      len(self.nearGoalRightPostDists))
        # average post distances
        if self.farLeftPostBearing is not None and \
                self.farRightPostBearing is not None:
            self.farAvgPostDist = (self.farLeftPostDist + self.farRightPostDist)*.5
        elif self.farLeftPostBearing is not None:
            self.farAvgPostDist = self.farLeftPostDist
        elif self.farRightPostBearing is not None:
            self.farAvgPostDist = self.farRightPostDist
        if self.nearLeftPostBearing is not None and \
                self.nearRightPostBearing is not None:
            self.nearAvgPostDist = (self.nearLeftPostDist + self.nearRightPostDist)*.5
        elif self.nearLeftPostBearing is not None:
            self.nearAvgPostDist = self.nearLeftPostDist
        elif self.nearRightPostBearing is not None:
            self.nearAvgPostDist = self.nearRightPostDist

    # Hack from US open 2012
    def dangerousBall(self):
        for mate in self.brain.teamMembers:
            if mate.playerNumber in [1] and mate.active:
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
                return self.chooseQuickFrontKick()
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

        # Determine which goal to aim at
        if self.farAvgPostDist != 0 and self.nearAvgPostDist != 0:
            if self.dangerousBall():
                rightPostBearing = self.farRightPostBearing
                leftPostBearing = self.farLeftPostBearing
            else:
                rightPostBearing = self.nearRightPostBearing
                leftPostBearing = self.nearLeftPostBearing
        elif self.farAvgPostDist != 0:
            rightPostBearing = self.farRightPostBearing
            leftPostBearing = self.farLeftPostBearing
        elif self.nearAvgPostDist != 0:
            if self.dangerousBall():
                # Can only see our own goal: Orbit to block.
                avgPostBearing = self.nearRightPostBearing + self.nearLeftPostBearing
                if avgPostBearing > 0:
                    # Orbit until blocking, not just 45 degrees. Then re-decide kick.
                    return kickLoc() #orbit left
                else:
                    return kickLoc() #orbit right
            else:
                rightPostBearing = self.nearRightPostBearing
                leftPostBearing = self.nearLeftPostBearing
        else:
            # Can't see any posts: orbit
            return self.kickLoc()

        # DEBUG printing
        print "rightPostBearing: ",rightPostBearing
        print "leftPostBearing:  ",leftPostBearing

        if rightPostBearing != 0 and leftPostBearing != 0:
            # Can't see any posts: orbit.
            return self.kickLoc()
        elif rightPostBearing != 0 and leftPostBearing != 0:
            # Can see both posts: shoot between them.
            leftScorePoint = rightPostBearing - 20
            rightScorePoint = leftPostBearing + 20

            if leftScorePoint < rightScorePoint:
                # less than 40 degrees of goal available to shoot in.
                leftScorePoint = ((rightPostBearing + leftPostBearing) / 2) + 5
                rightScorePoint = leftScorePoint - 10
        elif rightPostBearing != 0:
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
            return self.chooseQuickFrontKick()
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
                return self.chooseQuickFrontKick()
        elif rightScorePoint > -90:
            # Quadrent 4
            if (0 - leftScorePoint) - (rightScorePoint + 90) < 0:
                return self.chooseQuickFrontKick()
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

    def chooseShortFrontKick(self):
        if self.kickWithLeftFoot()
            return kicks.LEFT_SHORT_STRAIGHT_KICK
        return kicks.RIGHT_SHORT_STRAIGHT_KICK

    def chooseQuickFrontKick(self):
        if self.kickWithLeftFoot()
            return kicks.LEFT_QUICK_STRAIGHT_KICK
        return kicks.RIGHT_QUICK_STRAIGHT_KICK

    def kickLoc(self):
        """
        returns kick using localization
        """

        # DEBUG printing
        print "In method kickLoc."

        my = self.brain.my
        if (my.h <= 45. and my.h >= -45.):
            return self.chooseQuickFrontKick()
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
        if self.kickWithLeftFoot():
            return kicks.LEFT_SHORT_BACK_KICK
        else:
            return kicks.RIGHT_SHORT_BACK_KICK

    def kickWithLeftFoot(self):
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
