import kicks
import objects as Objects
import noggin_constants as constants
import math

DEBUG_KICK_DECISION = False
USE_LOC = True

class KickInformation:
    """
    Class to hold all the things we need to decide a kick
    """
    def __init__(self, brain):
#        self.decider = decider
        self.brain = brain

        # Set goal distance depending on type of field being used
        if constants.USING_LAB_FIELD:
            self.closeGoalThresh = 250
        else:
            self.closeGoalThresh = 450

        # If true, we will always aim for the center of the goal.
        # If false, we will aim for the corners of the goal.
        self.aimCenter = True

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

        self.farGoalieRed = 0
        self.farGoalieNavy = 0
        self.nearGoalieRed = 0
        self.nearGoalieNavy = 0
        # Currently, far goals are inaccurate for goalie detection.
        self.farGoalieOwn = False
        self.nearGoalieOwn = False

        self.dangerousBallCount = 0

        self.haveData = False

        self.aimAtOtherGoal = False

    def collectData(self):
        """
        Collect info on any observed goals
        """
        self.haveData = True

        if self.dangerousBall():
            self.dangerousBallCount += 1

        if self.brain.yglp.on:
            if self.brain.yglp.certainty == 2: # HACK for early messages, means _SURE
                if self.brain.yglp.distance > self.closeGoalThresh:
                    self.farGoalLeftPostBearings.append(self.brain.yglp.bearing_deg)
                    self.farGoalLeftPostDists.append(self.brain.yglp.distance)
                    if self.brain.yglp.red_goalie:
                        self.farGoalieRed += 1
                    elif self.brain.yglp.navy_goalie:
                        self.farGoalieNavy += 1
                if self.brain.yglp.distance < self.closeGoalThresh:
                    self.nearGoalLeftPostBearings.append(self.brain.yglp.bearing_deg)
                    self.nearGoalLeftPostDists.append(self.brain.yglp.distance)
                    if self.brain.yglp.red_goalie:
                        self.nearGoalieRed += 1
                    elif self.brain.yglp.navy_goalie:
                        self.nearGoalieNavy += 1

        if self.brain.ygrp.on:
            if self.brain.ygrp.certainty == 2: # HACK for early messages, means _SURE
                if self.brain.ygrp.distance > self.closeGoalThresh:
                    self.farGoalRightPostBearings.append(self.brain.ygrp.bearing_deg)
                    self.farGoalRightPostDists.append(self.brain.ygrp.distance)
                    if self.brain.ygrp.red_goalie:
                        self.farGoalieRed += 1
                    elif self.brain.ygrp.navy_goalie:
                        self.farGoalieNavy += 1
                if self.brain.ygrp.distance < self.closeGoalThresh:
                    self.nearGoalRightPostBearings.append(self.brain.ygrp.bearing_deg)
                    self.nearGoalRightPostDists.append(self.brain.ygrp.distance)
                    if self.brain.ygrp.red_goalie:
                        self.nearGoalieRed += 1
                    elif self.brain.ygrp.navy_goalie:
                        self.nearGoalieNavy += 1

    def hasEnoughInformation(self):
        """
        Can we stop panning and kick sooner?
        Note: minimum sightings here must be greater than minimums
              in calculateDataAverages method.
        """
        # If we're using loc to make kick decision, return true immediately.
        if USE_LOC:
            return True

        # If we've seen a pair of near posts with very good certainty, we can kick.
        seenPostPair = ((len(self.nearGoalLeftPostBearings) > 10 and
                        len(self.nearGoalRightPostBearings) > 10))

        # If we've seen a single post, and heard that it's a dangerous ball.
        nearPostAndDangerous = (self.dangerousBallCount > 5 and
                                (len(self.nearGoalLeftPostBearings) > 10 or
                                 len(self.nearGoalRightPostBearings) > 10 or
                                 len(self.farGoalLeftPostBearings) > 10 or
                                 len(self.farGoalRightPostBearings) > 10))

        if DEBUG_KICK_DECISION:
            if seenPostPair:
                print "Already seen a pair of near posts: abort pan."
            if nearPostAndDangerous:
                print "Already seen a post, and heard dangerous ball: abort pan."

        return (seenPostPair or nearPostAndDangerous)

    def calculateDataAverages(self):
        """
        calculates averages based on data collected
        """
        if not self.haveData:
            return

        # if DEBUG_KICK_DECISION:
        #     print "Total far goal sightings (sum both posts): ",\
        #         len(self.farGoalLeftPostBearings)+len(self.farGoalRightPostBearings)
        #     print "Total near goal sightings (sum both posts): ",\
        #         len(self.nearGoalLeftPostBearings)+len(self.nearGoalRightPostBearings)

        # bearing averages
        # Need more than 7 frames of each post to consider it "real".
        if len(self.farGoalLeftPostBearings) > 7:
            self.farLeftPostBearing = (sum(self.farGoalLeftPostBearings) /
                                       len(self.farGoalLeftPostBearings))
        if len(self.farGoalRightPostBearings) > 7:
            self.farRightPostBearing = (sum(self.farGoalRightPostBearings) /
                                        len(self.farGoalRightPostBearings))
        if len(self.nearGoalLeftPostBearings) > 7:
            self.nearLeftPostBearing = (sum(self.nearGoalLeftPostBearings) /
                                        len(self.nearGoalLeftPostBearings))
        if len(self.nearGoalRightPostBearings) > 7:
            self.nearRightPostBearing = (sum(self.nearGoalRightPostBearings) /
                                         len(self.nearGoalRightPostBearings))

        # distance averages
        if len(self.farGoalLeftPostDists) > 7:
            self.farLeftPostDist = (sum(self.farGoalLeftPostDists) /
                                    len(self.farGoalLeftPostDists))
        if len(self.farGoalRightPostDists) > 7:
            self.farRightPostDist = (sum(self.farGoalRightPostDists) /
                                     len(self.farGoalRightPostDists))
        if len(self.nearGoalLeftPostDists) > 7:
            self.nearLeftPostDist = (sum(self.nearGoalLeftPostDists) /
                                     len(self.nearGoalLeftPostDists))
        if len(self.nearGoalRightPostDists) > 7:
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

        # if DEBUG_KICK_DECISION:
        #     print "near reds: ",self.nearGoalieRed
        #     print "near navys: ",self.nearGoalieNavy

        # Determine visual dangerous goalie
        # Note that the values should be double the sightings:
        #  one for each post for each frame it is seen.
        if ((self.farGoalieRed > 5 and
             self.brain.gameController.teamColor == constants.teamColor.TEAM_RED) or
            (self.farGoalieNavy > 5 and
             self.brain.gameController.teamColor == constants.teamColor.TEAM_BLUE)):
            self.farGoalieOwn = True
        if ((self.nearGoalieRed > 5 and
             self.brain.gameController.teamColor == constants.teamColor.TEAM_RED) or
            (self.nearGoalieNavy > 5 and
             self.brain.gameController.teamColor == constants.teamColor.TEAM_BLUE)):
            self.nearGoalieOwn = True

    # Hack from US open 2012
    def dangerousBall(self):
        for mate in self.brain.teamMembers:
            if mate.playerNumber in [1] and mate.active:
                if mate.ballOn:# and mate.ballDist < self.closeGoalThresh+50:
                    # Goalie can't really see past midfield, so don't worry about distance 4/19
                    return True

        return False


    def shoot(self):
        """
        returns the kick we should do in a shooting situation
        """
        kick = None

        # Is loc good enough for a kick decision?
        # Need to use aimCenter in decision.
        if USE_LOC:

            # Get the bearing for the shot, i.e. from the ball to the goal.
            # Note: currently always aimCenter
            goalCenter = Objects.Location(constants.FIELD_WHITE_RIGHT_SIDELINE_X,
                                          constants.CENTER_FIELD_Y)
            goalLeft   = Objects.Location(constants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                                          constants.LANDMARK_OPP_GOAL_LEFT_POST_Y)
            goalRight  = Objects.Location(constants.LANDMARK_OPP_GOAL_RIGHT_POST_X,
                                          constants.LANDMARK_OPP_GOAL_RIGHT_POST_Y)

            ballLocation = Objects.Location(self.brain.ball.x, self.brain.ball.y)
            closeShot = (self.brain.loc.distTo(goalCenter) < 100) # within 1 meter
            ourHalf = (self.brain.loc.x < constants.MIDFIELD_X) # on our half of the field

            headingBallToGoalCenter = ballLocation.headingTo(goalCenter)
            headingBallToGoalLeft   = ballLocation.headingTo(goalLeft)
            headingBallToGoalRight  = ballLocation.headingTo(goalRight)

            if DEBUG_KICK_DECISION:
                print "Heading from the ball to the goal center: " + str(headingBallToGoalCenter)
                print "My global heading on the field: " + str(self.brain.loc.h)

            # Assume our heading at the ball will equal our current heading
            # We shouldn't be spinning at this point, so the assumption is valid.
            # Note: all headings are in degrees at this point.
            bearingForKick = headingBallToGoalCenter - self.brain.loc.h
            bearingKickLeft = headingBallToGoalLeft - self.brain.loc.h
            bearingKickRight = headingBallToGoalRight - self.brain.loc.h

            bearingLimitLeft = headingBallToGoalLeft - headingBallToGoalCenter
            bearingLimitRight = headingBallToGoalRight - headingBallToGoalCenter

            if not closeShot:

                if DEBUG_KICK_DECISION:
                    print ("Acceptable bearing range for kick: " + str(bearingLimitLeft) +
                           "/" + str(bearingLimitRight))

                kickBearings = [[0, 0], [70, 0], [-70, 0]] # straight, right side, left side
                for b in kickBearings:
                    adjustedBearingLeft = bearingKickLeft - b[0]
                    adjustedBearingRight = bearingKickRight - b[0]

                    if adjustedBearingLeft > 0 and adjustedBearingRight < 0:
                        #goal is in front of us
                        b[1] = 0
                    elif adjustedBearingLeft < 0 and adjustedBearingRight > 0:
                        #goal is behind us
                        if math.fabs(adjustedBearingLeft) < math.fabs(adjustedBearingRight):
                            #use the left bearing
                            b[1] = -adjustedBearingLeft
                        else:
                            #use the right bearing
                            b[1] = -adjustedBearingRight
                    elif adjustedBearingRight > 0:
                        #goal is to our left
                        b[1] = -adjustedBearingRight
                    elif adjustedBearingLeft < 0:
                        #goal is to our right
                        b[1] = -adjustedBearingLeft

                minimumBearing = min(map(math.fabs, [x[1] for x in kickBearings]))
                if minimumBearing == math.fabs(kickBearings[0][1]):
                    # choose a straight kick
                    kick = self.chooseShortFrontKick()
                elif minimumBearing == math.fabs(kickBearings[1][1]):
                    # choose a right side kick
                    kick = kicks.RIGHT_SIDE_KICK
                elif minimumBearing == math.fabs(kickBearings[2][1]):
                    # choose a left side kick
                    kick = kicks.LEFT_SIDE_KICK
                else:
                    # huh?
                    print "Didn't choose a kick..."
                kick.h = minimumBearing

                # If we're defending near our goal box, just kick it: clearing the ball
                # is more important than being super accurate.
                if (self.brain.loc.x < constants.LANDMARK_MY_FIELD_CROSS[0] and
                    self.brain.loc.y < constants.MY_GOALBOX_TOP_Y and
                    self.brain.loc.y > constants.MY_GOALBOX_BOTTOM_Y):
                    kick.h = 0
            else: # if closeShot is True
                if DEBUG_KICK_DECISION:
                    print "I am shooting very close to the opponent's goal."

                if DEBUG_KICK_DECISION:
                    print ("Acceptable bearing range for kick: " + str(bearingLimitLeft) +
                           "/" + str(bearingLimitRight))

                if (bearingKickLeft - bearingKickRight) > 60:
                    # even an inaccurate straight kick will work
                    if (30 > bearingKickLeft and -30 < bearingKickLeft):
                        #choose a straight kick with no orbit NOW!
                        kick = self.chooseShortFrontKick()
                        kick.h = 0
                        return kick
                    elif (bearingKickLeft < 30):
                        # closer to left limit
                        straightBearing = 30 - bearingKickLeft
                    else: #assert bearingLimitRight > -30
                        # closer to right limit
                        straightBearing = -30 - bearingKickRight
                else:
                    # aim for the center and hope we're accurate enough
                    straightBearing = 0 - bearingForKick

                if (bearingKickLeft - bearingKickRight > 35):
                    # even an inaccurate side kick will work
                    if (95 > bearingKickLeft and 60 < bearingKickRight):
                        #choose a right side kick with no orbit NOW!
                        kick = kicks.RIGHT_SIDE_KICK
                        kick.h = 0
                        return kick
                    elif (-60 > bearingKickLeft and -95 < bearingKickRight):
                        #choose a left side kick with no orbit!
                        kick = kicks.LEFT_SIDE_KICK
                        kick.h = 0
                        return kick

                    if (bearingKickLeft < 95):
                        # right side kick is closer to left limit
                        rightSideBearing = 95 - bearingKickLeft
                    else: #assert bearingKickRight > 60
                        rightSideBearing = 60 - bearingKickRight

                    if (bearingKickLeft < -60):
                        # left side kick is closer to the left limit
                        leftSideBearing = -60 - bearingKickLeft
                    else: #assert bearingKickRight > -95
                        leftSideBearing = -95 - bearingKickRight
                else:
                    # aim for the center and hope we're accurate enough
                    rightSideBearing = 70 - bearingForKick
                    leftSideBearing = -70 - bearingForKick

                # Currently, we ignore the possibility of doing a back kick here.

                # the kick bearing that has least magnitude should be chosen
                kickList = sorted([straightBearing, leftSideBearing, rightSideBearing], key=math.fabs)
                if (kickList[0] == straightBearing):
                    kick = self.chooseShortFrontKick()
                    kick.h = straightBearing
                elif (kickList[0] == rightSideBearing):
                    kick = kicks.RIGHT_SIDE_KICK
                    kick.h = rightSideBearing
                elif (kickList[0] == leftSideBearing):
                    kick = kicks.LEFT_SIDE_KICK
                    kick.h = leftSideBearing

            # Make sure heading is an int before passing it to the orbit.
            kick.h = int(kick.h)

            if DEBUG_KICK_DECISION:
                print "Returning a kick with heading: " + str(kick.h)

            return kick


        # Loc is bad- use only visual information to choose a kick.
        if DEBUG_KICK_DECISION:
            print "Using vision for kick decision."
            print "Dangerous ball count: ",self.dangerousBallCount
            print "Own goalie in near goal? ",self.nearGoalieOwn

        # Determine which goal to aim at
        if self.farAvgPostDist != 0 and self.nearAvgPostDist != 0:
            # Goalie detection too easily fooled.
            #  Screw it. We need it anyway.
            if (self.dangerousBallCount > 5 or
                self.nearGoalieOwn):
                # Don't aim at the near goal.
                rightPostBearing = self.farRightPostBearing
                leftPostBearing = self.farLeftPostBearing
            else:
                # Aim at the near goal.
                rightPostBearing = self.nearRightPostBearing
                leftPostBearing = self.nearLeftPostBearing
        elif self.farAvgPostDist != 0:
            # Only saw far goal? Kick towards it and hope.
            # Goalie detection too unreliable for far goal?
            rightPostBearing = self.farRightPostBearing
            leftPostBearing = self.farLeftPostBearing
        elif self.nearAvgPostDist != 0:
            # Goalie detection too easily fooled.
            #  Screw it. We need it anyway.
            if (self.dangerousBallCount > 5 or
                self.nearGoalieOwn):

                # Can only see our own goal: Use goalie to make decision
                if self.dangerousBallCount > 5:
                    if DEBUG_KICK_DECISION:
                        print "Doing a goalie based clearing kick."
                    return self.goalieBasedKick()

                # Saw two posts: use them to triangulate.
                if self.nearRightPostBearing != 0 and self.nearLeftPostBearing != 0:
                    if DEBUG_KICK_DECISION:
                        print "Saw two own posts. Triangulating and clearing."
                    return self.triangulateClearKick()

                # Saw one post: oh god, what now?
                # Default to no-info orbit.
                """
                if self.nearRightPostBearing != 0:
                    pass
                else:
                    pass
                """
                kick = kicks.ORBIT_KICK_POSITION
                kick.h = 45
                return kick
            else:
                rightPostBearing = self.nearRightPostBearing
                leftPostBearing = self.nearLeftPostBearing
        else:
            # No information at all? Orbit arbitrarily and try again.
            kick = kicks.ORBIT_KICK_POSITION
            kick.h = 45
            return kick

        if DEBUG_KICK_DECISION:
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

        if DEBUG_KICK_DECISION:
            print "rightScorePoint: ",rightScorePoint
            print "leftScorePoint:  ",leftScorePoint

        # If any kick is currently valid, choose that kick.
        # Note: this ignores the aimCenter distinction.
        if leftScorePoint > 0 and rightScorePoint < 0:
            kick = self.chooseQuickFrontKick()
        elif leftScorePoint > 70 and rightScorePoint < 70:
            kick = kicks.RIGHT_SIDE_KICK
        elif leftScorePoint > -70 and rightScorePoint < -70:
            kick = kicks.LEFT_SIDE_KICK
        elif leftScorePoint < -90 and rightScorePoint > 90:
            kick =self.chooseBackKick()

        # Did we pick a kick yet?
        if kick is not None:
            kick.h = 0 # Straight is fine- don't orbit.
            return kick

        # Choose whichever kick is closest to being between the score points.
        # Note: no kick bearing is between the posts, so they are all
        #   to the right of the rightScorePoint or left of leftScorePoint.
        # If not aimCenter, pick whichever kick is closest to either score point.
        avgScorePoint = int((rightScorePoint + leftScorePoint) * .5)

        if DEBUG_KICK_DECISION:
            print "Didn't choose a 0 heading kick.\navgScorePoint: ",avgScorePoint

        if rightScorePoint > 70:
                # Quadrant 2
            if (180 - leftScorePoint) - (rightScorePoint - 70) < 0:
                    #Closer to the leftScorePoint
                kick = self.chooseBackKick()
                if self.aimCenter:
                    kick.h = 180 - avgScorePoint
                else:
                    kick.h = 180 - leftScorePoint
            else:
                kick = kicks.RIGHT_SIDE_KICK
                if self.aimCenter:
                    kick.h = 70 - avgScorePoint
                else:
                    kick.h = 70 - rightScorePoint
        elif rightScorePoint > 0:
            # Quadrant 1
            if (70 - leftScorePoint) - (rightScorePoint - 0) < 0:
                kick = kicks.RIGHT_SIDE_KICK
                if self.aimCenter:
                    kick.h = 70 - avgScorePoint
                else:
                    kick.h = 70 - leftScorePoint
            else:
                kick = self.chooseQuickFrontKick()
                if self.aimCenter:
                    kick.h = 0 - avgScorePoint
                else:
                    kick.h = 0 - rightScorePoint
        elif rightScorePoint > -70:
            # Quadrant 4
            if (0 - leftScorePoint) - (rightScorePoint + 70) < 0:
                kick = self.chooseQuickFrontKick()
                if self.aimCenter:
                    kick.h = 0 - avgScorePoint
                else:
                    kick.h = 0 - leftScorePoint
            else:
                kick = kicks.LEFT_SIDE_KICK
                if self.aimCenter:
                    kick.h = -70 - avgScorePoint
                else:
                    kick.h = -70 - rightScorePoint
        else:
            # Quadrant 3
            if (-70 - leftScorePoint) - (rightScorePoint + 180) < 0:
                kick = kicks.LEFT_SIDE_KICK
                if self.aimCenter:
                    kick.h = -70 - avgScorePoint
                else:
                    kick.h = -70 - leftScorePoint
            else:
                kick = self.chooseBackKick()
                if self.aimCenter:
                    kick.h = -180 - avgScorePoint
                else:
                    kick.h = -180 - rightScorePoint

        if kick is not None:
            return kick

        if DEBUG_KICK_DECISION:
            print "Somehow got to end without a kick..."

        # If all else fails, orbit and re-decide.
        # Note: this case should already be covered above,
        #  but is repeated for safety.
        kick = kicks.ORBIT_KICK_POSITION
        kick.h = 45
        return kick

    def goalieBasedKick(self):
        # Assert: Neither far post was seen.
        #         At least one near post was seen.
        #         Goalie registered dangerous balls.
        #         Either Goalie can currently see the ball, or not.
        goalieBearing = 0
        goalieDist = 0
        kick = None

        for mate in self.brain.teamMembers:
            if mate.playerNumber == 1:
                goalieBearing = mate.ballBearing
                goalieDist = mate.ballDist

        # Sanity check.
        if goalieBearing == 0 or goalieDist == 0:
            # Something went wrong. Abort and default to no info orbit.
            kick = kicks.ORBIT_KICK_POSITION
            kick.h = 45
            return kick

        # Calculate coordinates of ball, assuming goalie is perfectly centered.
        goalieX = constants.FIELD_WHITE_LEFT_SIDELINE_X
        goalieY = constants.CENTER_FIELD_Y
        diffX = math.cos(math.radians(goalieBearing)) * goalieDist
        diffY = math.sin(math.radians(goalieBearing)) * goalieDist
        ballX = goalieX + diffX
        ballY = goalieY + diffY
        ballLocation = Objects.Location(ballX, ballY)

        # Calculate your bearing on the post with more sightings
        if len(self.nearGoalRightPostBearings) > len(self.nearGoalLeftPostBearings):
            # Use the right post
            relPostBearing = self.nearRightPostBearing
            ballToPost = ballLocation.relativeLocationOf(Objects.Location(constants.FIELD_WHITE_LEFT_SIDELINE_X,constants.LANDMARK_BLUE_GOAL_BOTTOM_POST_Y))
        else:
            # Use the left post
            relPostBearing = self.nearLeftPostBearing
            ballToPost = ballLocation.relativeLocationOf(Objects.Location(constants.FIELD_WHITE_LEFT_SIDELINE_X,constants.LANDMARK_BLUE_GOAL_TOP_POST_Y))

        ballToPostBearing = ballToPost.bearing
        myGlobalHeading = ballToPostBearing - relPostBearing
        if myGlobalHeading < -180:
            myGlobalHeading += 360
        elif myGlobalHeading > 180:
            myGlobalHeading -= 360

        return self.chooseClearKick(myGlobalHeading, ballX, ballY)


    def chooseClearKick(self, myGlobalHeading, ballX, ballY):
        # Assert: I have my global heading and coordinates of the ball.

        if DEBUG_KICK_DECISION:
            print "chose a clear kick."
            print "myGlobalHeading: ",myGlobalHeading
            print "ballX: ",ballX
            print "ballY: ",ballY

        # Determine which kick I should do.
        if myGlobalHeading < -135 or myGlobalHeading > 135:
            kick = self.chooseBackKick()
            #should I orbit?
            if ballY < constants.MIDFIELD_Y and myGlobalHeading > 135:
                kick.h = myGlobalHeading - 180 # to your right
            elif ballY > constants.MIDFIELD_Y and myGlobalHeading < -135:
                kick.h = myGlobalHeading + 180 # to your left
            else:
                kick.h = 0
        elif myGlobalHeading < -45:
            kick = kicks.RIGHT_SIDE_KICK
            #should I orbit?
            if ballY < constants.LANDMARK_BLUE_GOAL_BOTTOM_POST_Y or \
                    ballY > constants.LANDMARK_BLUE_GOAL_TOP_POST_Y:
                kick.h = myGlobalHeading + 90
            else:
                kick.h = 0
        elif myGlobalHeading > 45:
            kick = kicks.LEFT_SIDE_KICK
            #should I orbit?
            if ballY < constants.LANDMARK_BLUE_GOAL_BOTTOM_POST_Y or \
                    ballY > constants.LANDMARK_BLUE_GOAL_TOP_POST_Y:
                kick.h = myGlobalHeading - 90
            else:
                kick.h = 0
        else:
            kick = self.chooseQuickFrontKick()
            #should I orbit?
            if (ballY < constants.MIDFIELD_Y and myGlobalHeading < 0) or \
                    (ballY > constants.MIDFIELD_Y and myGlobalHeading > 0):
                kick.h = myGlobalHeading
            else:
                kick.h = 0

        # Make sure the heading is an int before passing it to nav as an orbit angle.
        kick.h = int(kick.h)
        return kick

    def triangulateClearKick(self):
        # Assert: Neither far post was seen.
        #         Both near posts were seen.
        #         Use near posts to hack a loc for ourselves.
        bearingDiff = self.nearLeftPostBearing - self.nearRightPostBearing
        leftDist = self.nearRightPostDist
        rightDist = self.nearLeftPostDist
        leftPostY = constants.LANDMARK_BLUE_GOAL_TOP_POST_Y
        rightPostY = constants.LANDMARK_BLUE_GOAL_BOTTOM_POST_Y
        farSideLenth = constants.CROSSBAR_CM_WIDTH

        # Should be generalized into a helper method somewhere.
        y = (math.pow(leftPostY,2) + math.pow(leftDist,2) - math.pow(rightPostY,2) - math.pow(rightDist,2)) / (2 * (leftPostY - rightPostY))
        x = math.sqrt(math.pow(leftDist,2) - math.pow((y-rightPostY),2))

        # Use locations to get global heading.
        post = Objects.Location(constants.FIELD_WHITE_LEFT_SIDELINE_X,constants.LANDMARK_BLUE_GOAL_TOP_POST_Y)
        me = Objects.Location(x,y)
        meToPost = me.relativeLocationOf(post)

        myGlobalHeading = meToPost.bearing - self.nearLeftPostBearing
        if myGlobalHeading < -180:
            myGlobalHeading += 360
        elif myGlobalHeading > 180:
            myGlobalHeading -= 360

        return self.chooseClearKick(myGlobalHeading, x, y)

    def chooseShortFrontKick(self):
        if self.kickWithLeftFoot():
            return kicks.LEFT_SHORT_STRAIGHT_KICK
        return kicks.RIGHT_SHORT_STRAIGHT_KICK

    def chooseQuickFrontKick(self):
        # If our goalie is inactive, always use short front kicks.
        # If we are using loc-based kicks, ignore the above.
        if not self.brain.player.penaltyKicking and not USE_LOC:
            for mate in self.brain.teamMembers:
                if ((mate.playerNumber == 1) and (mate.active is False)):
                    return self.chooseShortFrontKick()

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
        if self.brain.my.y > constants.CENTER_FIELD_Y:
            return kicks.LEFT_SHORT_BACK_KICK
        else:
            return kicks.RIGHT_SHORT_BACK_KICK
        """
        if self.kickWithLeftFoot():
            return kicks.LEFT_LONG_BACK_KICK
        else:
            return kicks.RIGHT_LONG_BACK_KICK

    def kickWithLeftFoot(self):
        """
        How we choose which kick to do when we're facing the ball. For now,
        simply pick the foot that's closer to the ball.
        """
        if self.brain.ball.bearing_deg > 0: # TODO should be relY but
            # no message for it yet.
            return True
        else:
            return False

    def checkKickingFoot(self, kick):
        """
        Takes a kick and decides which foot to use.
        """
        if kick == kicks.RIGHT_SIDE_KICK or \
                kick == kicks.LEFT_SIDE_KICK:
            # Side kicks can't change which foot they use.
            return kick
        elif kick == kicks.LEFT_SHORT_STRAIGHT_KICK or \
                kick == kicks.RIGHT_SHORT_STRAIGHT_KICK:
            if self.kickWithLeftFoot():
                kick = kicks.LEFT_SHORT_STRAIGHT_KICK
            else:
                kick = kicks.RIGHT_SHORT_STRAIGHT_KICK
            return kick
        elif kick == kicks.LEFT_STRAIGHT_KICK or \
                kick == kicks.RIGHT_STRAIGHT_KICK:
            if self.kickWithLeftFoot():
                kick = kicks.LEFT_STRAIGHT_KICK
            else:
                kick = kicks.RIGHT_STRAIGHT_KICK
            return kick
        elif kick == kicks.LEFT_LONG_BACK_KICK or \
                kick == kicks.RIGHT_LONG_BACK_KICK:
            if self.kickWithLeftFoot():
                kick = kicks.LEFT_LONG_BACK_KICK
            else:
                kick = kicks.RIGHT_LONG_BACK_KICK
            return kick
        else:
            # If the case was missed, just return the original kick.
            return kick

    def shouldKickPanRight(self):
        """
        Decide whether to pan left or right first for kick decision.
        """
        heading = self.brain.loc.h
        xPosition = self.brain.loc.x

        if xPosition < 520:
            # Blue half of field.
            if heading > 0:
                return False
            else:
                return True
        else:
            # Yellow half of field.
            if heading > 0:
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
        return s
