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
                if self.brain.yglp.vis.dist > 300:
                    self.farGoalLeftPostBearings.append(self.brain.yglp.vis.bearing)
                    self.farGoalLeftPostDists.append(self.brain.yglp.vis.dist)
                if self.brain.yglp.vis.dist < 300:
                    self.nearGoalLeftPostBearings.append(self.brain.yglp.vis.bearing)
                    self.nearGoalLeftPostDists.append(self.brain.yglp.vis.dist)

        if self.brain.ygrp.vis.on:
            self.sawGoal = True
            if self.brain.ygrp.vis.certainty == vision.certainty._SURE:
                if self.brain.ygrp.vis.dist > 300:
                    self.farGoalRightPostBearings.append(self.brain.ygrp.vis.bearing)
                    self.farGoalRightPostDists.append(self.brain.ygrp.vis.dist)
                if self.brain.ygrp.vis.dist < 300:
                    self.nearGoalRightPostBearings.append(self.brain.ygrp.vis.bearing)
                    self.nearGoalRightPostDists.append(self.brain.ygrp.vis.dist)

    def calculateDataAverages(self):
        """
        calculates averages based on data collected
        """
        if not self.haveData:
            return

        #DEBUG printing
        print "Total far goal sightings (sum both posts): ",len(self.farGoalLeftPostBearings)+len(self.farGoalRightPostBearings)
        print "Total near goal sightings (sum both posts): ",len(self.nearGoalLeftPostBearings)+len(self.nearGoalRightPostBearings)

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
            relLocationBallToGoal = self.brain.ball.loc.relativeLocationOf(Location(670,270))
            bearingBallToGoal = relLocationBallToGoal.bearing
            # Assume our bearing at the ball will equal our current bearing
            relLocationMeToBall = self.brain.my.relativeLocationOf(self.brain.ball.loc)
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
        #DEBUG printing
        print "Using vision for kick decision."
        print "Dangerous ball? ",self.dangerousBall()

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
                avgPostBearing = self.nearRightPostBearing + self.nearLeftPostBearing / 2
                orbitAngle = -180 + avgPostBearing
                if orbitAngle < -180:
                    orbitAngle += 180
                orbitAngle = int(orbitAngle)

                # Orbit until blocking, not just 45 degrees. Then re-decide kick.
                kick = kicks.ORBIT_KICK_POSITION
                kick.h = orbitAngle
                return kick
            else:
                rightPostBearing = self.nearRightPostBearing
                leftPostBearing = self.nearLeftPostBearing
        else:
            # Can't see any posts: orbit
            kick = kicks.ORBIT_KICK_POSITION
            kick.h = 45
            return kick

        # DEBUG printing
        print "farRightPostBearing: ",self.farRightPostBearing
        print "farLeftPostBearing: ",self.farLeftPostBearing
        print "nearRightPostBearing: ",self.nearRightPostBearing
        print "nearLeftPostBearing: ",self.nearLeftPostBearing
        print "rightPostBearing: ",rightPostBearing
        print "leftPostBearing:  ",leftPostBearing

        if rightPostBearing == 0 and leftPostBearing == 0:
            # Can't see any posts: orbit.
            # Note: this case should already be covered above,
            #  but is repeated for safety.
            kick = kicks.ORBIT_KICK_POSITION
            kick.h = 45
            return kick
        elif rightPostBearing != 0 and leftPostBearing != 0:
            # Can see both posts: shoot between them.
            leftScorePoint = rightPostBearing - 10
            rightScorePoint = leftPostBearing + 10

            if leftScorePoint < rightScorePoint:
                # less than 20 degrees of goal available to shoot in.
                leftScorePoint = ((rightPostBearing + leftPostBearing) / 2) + 5
                rightScorePoint = leftScorePoint - 10
        elif rightPostBearing != 0:
            # Can only see the right post.
            leftScorePoint = rightPostBearing - 15
            rightScorePoint = leftScorePoint - 10
        else:
            # Can only see the left post.
            rightScorePoint = leftPostBearing + 15
            leftScorePoint = rightScorePoint + 10

        # DEBUG printing
        print "rightScorePoint: ",rightScorePoint
        print "leftScorePoint:  ",leftScorePoint

        kick = None

        # If any kick is currently valid, choose that kick.
        if leftScorePoint > 0 and rightScorePoint < 0:
            kick = self.chooseQuickFrontKick()
        elif leftScorePoint > 90 and rightScorePoint < 90:
            kick = kicks.RIGHT_SIDE_KICK
        elif leftScorePoint > -90 and rightScorePoint < -90:
            kick = kicks.LEFT_SIDE_KICK
        elif leftScorePoint < -90 and rightScorePoint > 90:
            kick =self.chooseBackKick()

        # Did we pick a kick yet?
        if kick is not None:
            kick.h = 0 # Straight is fine.
            return kick

        # Choose whichever kick is closest to being between the score points.
        # Note: no kick bearing is between the posts, so they are all
        #   to the right of the rightScorePoint or left of leftScorePoint.
        avgScorePoint = int((rightScorePoint + leftScorePoint) * .5)

        #DEBUG printing
        print "Didn't choose a 0 heading kick.\navgScorePoint: ",avgScorePoint

        if rightScorePoint > 90:
                # Quadrent 2
            if (180 - leftScorePoint) - (rightScorePoint - 90) < 0:
                    #Closer to the leftScorePoint
                kick = self.chooseBackKick()
                kick.h = 1 #HACK
            else:
                kick = kicks.RIGHT_SIDE_KICK
                kick.h = 90 - avgScorePoint
        elif rightScorePoint > 0:
            # Quadrent 1
            if (90 - leftScorePoint) - (rightScorePoint - 0) < 0:
                kick = kicks.RIGHT_SIDE_KICK
                kick.h = 90 - avgScorePoint
            else:
                kick = self.chooseQuickFrontKick()
                kick.h = 0 - avgScorePoint
        elif rightScorePoint > -90:
            # Quadrent 4
            if (0 - leftScorePoint) - (rightScorePoint + 90) < 0:
                kick = self.chooseQuickFrontKick()
                kick.h = 0 - avgScorePoint
            else:
                kick = kicks.LEFT_SIDE_KICK
                kick.h = -90 - avgScorePoint
        else:
            # Quadrent 3
            if (-90 - leftScorePoint) - (rightScorePoint + 180) < 0:
                kick = kicks.LEFT_SIDE_KICK
                kick.h = -90 - avgScorePoint
            else:
                kick = self.chooseBackKick()
                kick.h = 1 #HACK

        if kick is not None:
            return kick

        #DEBUG printing
        print "Somehow got to end without a kick..."

        # If all else fails, orbit and re-decide.
        # Note: this case should already be covered above,
        #  but is repeated for safety.
        kick = kicks.ORBIT_KICK_POSITION
        kick.h = 45
        return kick

    def chooseShortFrontKick(self):
        if self.kickWithLeftFoot():
            return kicks.LEFT_SHORT_STRAIGHT_KICK
        return kicks.RIGHT_SHORT_STRAIGHT_KICK

    def chooseQuickFrontKick(self):
        if self.kickWithLeftFoot():
            return kicks.LEFT_STRAIGHT_KICK
        return kicks.RIGHT_STRAIGHT_KICK

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
        if self.farLeftPostBearing is not None:
            s += ("Far left post bearing is: " + str(self.farLeftPostBearing) +
                  " dist is: " + str(self.farLeftPostDist) + "\n")
        if self.farRightPostBearing is not None:
            s += ("Far right post bearing is: " + str(self.farRightPostBearing)
                  + " dist is: " + str(self.farRightPostDist) +  "\n")
        if self.nearLeftPostBearing is not None:
            s += ("Near left post bearing is: " + str(self.nearLeftPostBearing) +
                  " dist is: " + str(self.nearLeftPostDist) + "\n")
        if self.nearRightPostBearing is not None:
            s += ("Near right post bearing is: " + str(self.nearRightPostBearing)
                  + " dist is: " + str(self.nearRightPostDist) +  "\n")
        if s == "":
            s = "No goal posts observed"
#        if self.kickObjective == constants.OBJECTIVE_SHOOT:
#            s += "\nObjective is: SHOOT"
#        if self.kickObjective == constants.OBJECTIVE_CLEAR:
#            s += "\nObjective is: CLEAR"
        return s
