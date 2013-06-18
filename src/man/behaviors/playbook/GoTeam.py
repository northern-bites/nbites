from math import (hypot, atan2, cos, sin, acos, asin)
from ..util import MyMath
from ..typeDefs import TeamMember
from . import PBConstants
from . import Strategies
from objects import Location, RobotLocation, RelRobotLocation
from math import degrees, ceil
import noggin_constants as NogginConstants
import time
from PlaybookTable import playbookTable

# ANSI terminal color codes
# http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html
RESET_COLORS_CODE = '\033[0m'
RED_COLOR_CODE = '\033[31m'
GREEN_COLOR_CODE = '\033[32m'
YELLOW_COLOR_CODE = '\033[33m'
BLUE_COLOR_CODE = '\033[34m'
PURPLE_COLOR_CODE = '\033[35m'
CYAN_COLOR_CODE = '\033[36m'

class GoTeam:
    """This is the class which controls all of our coordinated
       behavior system."""
    def __init__(self, brain):
        self.brain = brain
        self.printStateChanges = True
        self.time = time.time()

        # Information about teammates
        #self.position = []
        self.me = self.brain.teamMembers[self.brain.playerNumber - 1]
        self.me.playerNumber = self.brain.playerNumber
        self.activeFieldPlayers = []
        self.numActiveFieldPlayers = 0
        self.kickoffFormation = 0
        self.timeSinceCaptureChase = 0
        self.subRoleSwitchTime = 0
        self.goalieChaserCount = 0
        self.willBeIllegalD = 0
        self.stopAvoidingBox = 0
        self.ellipse = Ellipse(PBConstants.LARGE_ELLIPSE_CENTER_X,
                               PBConstants.LARGE_ELLIPSE_CENTER_Y,
                               PBConstants.LARGE_ELLIPSE_HEIGHT,
                               PBConstants.LARGE_ELLIPSE_WIDTH)
        self.potentialBallX = -1
        self.potentialBallXFrames = 0
        self.potentialBallY = -1
        self.potentialBallYFrames = 0
        self.lastBallX = -1
        self.lastBallY = -1


        # Goalie
        self.goalieIsActive = False
        self.shouldPositionLeftCounter = 0
        self.shouldPositionRightCounter = 0
        self.shouldPositionCenterCounter = 0
        self.shouldSaveCounter = 0
        self.shouldChaseCounter = 0
        self.shouldStopChaseCounter = 0
        self.shouldStopSaveCounter = 0

    def run(self, play):
        """We run this each frame to get the latest info"""
        self.aPrioriTeammateUpdate()

        if self.brain.player.gameState == 'gameReady':
            # Change which wing is forward based on the opponents score
            # TODO: implement this
            pass

        play.changed = False
        self.strategize(play)

        # If the play has changed, print.
        self.updateStateInfo(play)

    def strategize(self, play):
        """
        creates a play, picks the strategy to run, returns the play after
        it is modified by Strategies
        """
        currentGCState = self.brain.player.gameState
        # We don't control anything in initial or finished
        if (currentGCState == 'gameInitial' or
            currentGCState == 'gameFinished'):
            if not play.isSubRole(PBConstants.INIT_SUB_ROLE):
                play.setStrategy(PBConstants.INIT_STRATEGY)
                play.setFormation(PBConstants.INIT_FORMATION)
                play.setRole(PBConstants.INIT_ROLE)
                play.setSubRole(PBConstants.INIT_SUB_ROLE)

            self.lastBallX = -1
            self.lastBallY = -1
            return

        # Have a separate strategy to easily deal with being penalized
        elif not self.me.active:
            if not play.isSubRole(PBConstants.PENALTY_SUB_ROLE):
                play.setStrategy(PBConstants.PENALTY_STRATEGY)
                play.setFormation(PBConstants.PENALTY_FORMATION)
                play.setRole(PBConstants.PENALTY_ROLE)
                play.setSubRole(PBConstants.PENALTY_SUB_ROLE)

            self.lastBallX = -1
            self.lastBallY = -1
            return

        # Sometimes the ball filter produces infinity values when starting.
        # HACK: Clip brain.ball.x and brain.ball.y
        if self.brain.ball.x == float("inf") or self.brain.ball.x == float("-inf"):
            self.brain.ball.x = NogginConstants.MIDFIELD_X
        if self.brain.ball.y == float("inf") or self.brain.ball.y == float("-inf"):
            self.brain.ball.y = NogginConstants.MIDFIELD_Y

        #print "y is: " + str(self.brain.ball.y)
        #print "pre-ceil: " + str((self.brain.ball.x - NogginConstants.GREEN_PAD_X) / PBConstants.TABLE_GRID_SIZE)
        #print "post-ceil: " + str(ceil((self.brain.ball.x - NogginConstants.GREEN_PAD_X) / PBConstants.TABLE_GRID_SIZE))

        #Which grid entry is the ball in?
        newBallX = int(ceil((self.brain.ball.x - NogginConstants.GREEN_PAD_X) / PBConstants.TABLE_GRID_SIZE))
        newBallY = int(ceil((self.brain.ball.y - NogginConstants.GREEN_PAD_Y) / PBConstants.TABLE_GRID_SIZE))

        # For robustness, clip these values
        newBallX = MyMath.clip(newBallX, 0, PBConstants.TABLE_GRID_WIDTH)
        newBallY = MyMath.clip(newBallY, 0, PBConstants.TABLE_GRID_HEIGHT)

        # If lastBallX or lastBallY is -1, our best guess is newBallX and newBallY
        if (self.lastBallX == -1 and self.lastBallY == -1):
            self.lastBallX = newBallX
            self.lastBallY = newBallY

        # Buffer the x coordinate for the grid to avoid rapid oscillations
        if newBallX != self.lastBallX:
            if newBallX == self.potentialBallX:
                self.potentialBallXFrames += 1
                if self.potentialBallXFrames > 10:
                    self.lastBallX = self.potentialBallX
            else:
                self.potentialBallX = newBallX
                self.potentialBallXFrames = 1

        # Buffer the y coordinate for the grid to avoid rapid oscillations
        if newBallY != self.lastBallY:
            if newBallY == self.potentialBallY:
                self.potentialBallYFrames += 1
                if self.potentialBallYFrames > 10:
                    self.lastBallY = self.potentialBallY
            else:
                self.potentialBallY = newBallY
                self.potentialBallYFrames = 1

        # Check for testing stuff
        # Not currently being called as of 6/4/13
        # Broken from playbook overhaul 6/17/13
        #elif PBConstants.TEST_DEFENDER:
        #    Strategies.sTestDefender(self, play)
        #elif PBConstants.TEST_OFFENDER:
        #    Strategies.sTestOffender(self, play)
        #elif PBConstants.TEST_CHASER:
        #    Strategies.sTestChaser(self, play)

        # Have a separate ready section to make things simpler
        elif (currentGCState == 'gameReady' or
              currentGCState =='gameSet'):
            Strategies.sReady(self, play)

        else:
            # Use the playbook table to determine position.
            self.priorityPositions(self.tableLookup(self.lastBallX, self.lastBallY), play)

        # # Now we look at game strategies
        # elif self.numActiveFieldPlayers == 0:
        #     Strategies.sNoFieldPlayers(self, play)
        # elif self.numActiveFieldPlayers == 1:
        #     Strategies.sOneField(self, play)
        # elif self.numActiveFieldPlayers == 2:
        #     Strategies.sTwoField(self, play)
        # elif self.numActiveFieldPlayers == 3:
        #     Strategies.sThreeField(self, play)
        # elif self.numActiveFieldPlayers == 4:
        #     Strategies.sWin(self, play)

    def tableLookup(self, ball_x, ball_y):
        """
        Given where we think the ball is on the field (from loc),
        use the playbook table to look up where we should position.
        @return: list of position tuples in order of priority.
        """
        entry = playbookTable[ball_x][ball_y]
        offset = 0
        positions = []

        if len(self.activeFieldPlayers) == 3:
            offset = 4
        elif len(self.activeFieldPlayers) == 2:
            offset = 7
        elif len(self.activeFieldPlayers) == 1:
            offset = 9

        for i in range(len(self.activeFieldPlayers)):
            if self.goalieIsActive:
                positions.append(entry[1][i+offset])
            else:
                positions.append(entry[0][i+offset])

        return positions

    def priorityPositions(self, positions, play):
        """
        Determine which player should go to each position.
        """
        locations = map(self.mapPositionToRobotLocation, positions)

        # Set the chaser!
        chaser_mate = self.determineChaser(play)
        if chaser_mate.playerNumber == self.brain.playerNumber:
            play.setRole(positions[len(locations)-1][3])
            play.setPosition(locations[len(locations)])

        # Find which active field player should go to each position
        firstPlayer = self.findClosestPlayer(locations[0], [chaser_mate])
        if firstPlayer == self.brain.playerNumber:
            play.setRole(positions[0][3])
            play.setPosition(locations[0])
        else:
            secondPlayer = self.findClosestPlayer(locations[1], [firstPlayer, chaser_mate])
            if secondPlayer == self.brain.playerNumber:
                play.setRole(positions[1][3])
                play.setPosition(locations[1])
            else:
                play.setRole(positions[2][3])
                play.setPosition(locations[2])

    def mapPositionToRobotLocation(self, position):
        """
        Position must be a tuple with x, y, heading, role.
        @return: that position as a Robot Location
        """
        return RobotLocation(position[0], position[1], position[2])

    def updateStateInfo(self, play):
        """
        Update information specific to the coordinated behaviors
        """
        # Print changes and Say changes
        if play.changed:
            if self.printStateChanges:
                self.printf("Play switched to " + play.__str__())
                self.me.updateMe() # make sure roles get set this frame

    ######################################################
    ############       Role Switching Stuff     ##########
    ######################################################
    def determineChaser(self, play):
        """return the team member who is the chaser"""
        chaser_mate = self.activeFieldPlayers[0]

        if PBConstants.DEBUG_DET_CHASER:
            self.printf("chaser det: me == #%g"% self.brain.playerNumber)

        # save processing time and skip the rest if we have the ball
        if self.brain.player.inKickingState:
            if PBConstants.DEBUG_DET_CHASER:
                self.printf("I should Chase")
            return self.me

        # scroll through the teammates
        for mate in self.activeFieldPlayers:
            if PBConstants.DEBUG_DET_CHASER:
                self.printf("\t mate #%g"% mate.playerNumber)

            # We can skip computation if the mate we are now considering
            # is the chaser_mate.
            if (mate == chaser_mate):
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("mate %g is chaser_mate already."
                                % mate.playerNumber)
                continue

            # If the mate is already at the ball, and will reach it first even if
            #  another mate is closer and sees the ball next frame, it should be
            #  chaser.
            elif (mate.hasBall() and
                  mate.chaseTime < TeamMember.BALL_OFF_PENALTY):
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("mate %g has ball" % mate.playerNumber)
                chaser_mate = mate

            else:
                if self.shouldCallOff(chaser_mate, mate):
                    if PBConstants.DEBUG_DET_CHASER:
                        self.printf("\t #%d @ %g < #%d @ %g, shouldCallOff" %
                                    (mate.playerNumber, mate.chaseTime,
                                     chaser_mate.playerNumber,
                                     chaser_mate.chaseTime))
                    chaser_mate = mate

                elif PBConstants.DEBUG_DET_CHASER:
                    self.printf (("\t #%d @ %g > #%d @ %g, didn't call off" %
                                  (mate.playerNumber, mate.chaseTime,
                                   chaser_mate.playerNumber,
                                   chaser_mate.chaseTime)))

        if PBConstants.DEBUG_DET_CHASER:
            self.printf ("\t ---- MATE %g WINS" % (chaser_mate.playerNumber))
        # returns teammate instance (could be mine)
        return chaser_mate

    def shouldCallOff(self, chaser_mate, mate):
        """Decides if mate shouldCallOff the chaser_mate"""
        # mate = A, chaser_mate = B.
        # A will become chaser_mate if:
        # [ (chaseTime(A) - chaseTime(B) < e) or
        #   (chaseTime(A) - chaseTime(B) < d and A is already chasing)]
        # Note: d > e
        # A is higher robot that has decided to be chaser.
        return(((mate.chaseTime - chaser_mate.chaseTime) <
                PBConstants.CALL_OFF_THRESH) or
               ((mate.chaseTime - chaser_mate.chaseTime) <
                PBConstants.STOP_CALLING_THRESH and
                mate.isTeammateRole(PBConstants.CHASER)))

    def getLeastWeightPosition(self, positions, mates = None):
        """Gets the position for the robot such that the distance
           all robot have to move is the least possible"""
        # if there is only one position return the position
        if len(positions) == 1 or mates == None:
            return positions[0]

        if len(positions) != len(mates)+1:
            self.printf ("Error in getLeastWeightPosition. Not equal numbers of mates/positions")
            return positions[0]

        # if we have two positions only two possibilites of positions
        elif len(positions) == 2:
            myDist1 = hypot(positions[0].toTupleXY()[0] - self.brain.loc.x,
                            positions[0].toTupleXY()[1] - self.brain.loc.y)
            myDist2 = hypot(positions[1].toTupleXY()[0] - self.brain.loc.x,
                            positions[1].toTupleXY()[1] - self.brain.loc.y)
            mateDist1 = hypot(positions[0].toTupleXY()[0] - mates[0].x,
                              positions[0].toTupleXY()[1] - mates[0].y)
            mateDist2 = hypot(positions[1].toTupleXY()[0] - mates[0].x,
                              positions[1].toTupleXY()[1] - mates[0].y)

            # Subrole hysteresis prevention does tie-breaking for us.
            # May need role hysteresis prevention.
            if myDist1 + mateDist2 == myDist2 + mateDist1:
                if self.me.playerNumber > mates[0].playerNumber:
                    return positions[0]
                else:
                    return positions[1]
            elif myDist1 + mateDist2 < myDist2 + mateDist1:
                return positions[0]
            else:
                return positions[1]

        # We have three positions
        # Pseudo 2d array to hold the 3 bots dists to the 3 positions
        elif len(positions) == 3:
            bot2, bot3, bot4 = [], [], []
            bot_distances = [bot2,bot3,bot4]

            # get distances from every bot every possible position
            for i,bot in enumerate(bot_distances):
                bot_number = i+2
                x, y = 0.0, 0.0
                # use either my estimate or teammates'
                if bot_number == self.me.playerNumber:
                    x = self.brain.loc.x
                    y = self.brain.loc.y
                else:
                    x = self.mates[bot_number-1].x
                    y = self.mates[bot_number-1].y

                for p in positions:
                    bot.append([hypot(p[0] - x, p[1] - y), p])

            # bot1, bot2, bot3 are now 2D arrays of size 3x2.
            # the first arg corresponds to which position the bot is picking
            # and the second arg is the dist(0) which is calculated above
            # or the actual Location(1) which is also an [x,y] array.

            # 6 possible weights and positions
            distances = [ (bot2[0][0] + bot3[1][0] + bot4[2][0],  # dist sum,
                           bot2[0][1],  bot3[1][1],  bot4[2][1]), # positions
                          (bot2[1][0] + bot3[2][0] + bot4[0][0],  # dist sum,
                           bot2[1][1],  bot3[2][1],  bot4[0][1]), # positions
                          (bot2[2][0] + bot3[0][0] + bot4[1][0],  # .
                           bot2[2][1],  bot3[0][1],  bot4[1][1]), # .
                          (bot2[1][0] + bot3[0][0] + bot4[2][0],  # .
                           bot2[1][1],  bot3[0][1],  bot4[2][1]), # .
                          (bot2[2][0] + bot3[1][0] + bot4[0][0],  # .
                           bot2[2][1],  bot3[1][1],  bot4[0][1]), # .
                          (bot2[0][0] + bot3[2][0] + bot4[1][0],  # dist sum,
                           bot2[0][1],  bot3[2][1],  bot4[1][1])] # positions

            # We must find the least weight choice from the 6 possibilities
            min_dist = NogginConstants.FIELD_WIDTH*3
            chosenPositions = None
            for i,d in enumerate(distances):
                if d[0] < min_dist:
                    min_dist = d[0]
                    chosenPositions = d

            # chosen Postitions is an array of size 4
            #where 1,2,3 are the positions

            # returns a Location
            return chosenPositions[self.me.playerNumber -1]

    #@param location: this is a RobotLocation
    def findClosestPlayer(self, location, exceptNumbers = []):
        """
        Using a simple model to determine time to reach the given location,
        find which active team player (not in exceptNumbers) has the
        minimum time to reach destination, and return their player number.
        """
        minimumTime = 10000
        playerNum = -1
        for mate in self.activeFieldPlayers:
            if mate.playerNumber in exceptNumbers:
                continue

            time = mate.determineTimeToDest(RobotLocation(mate.x,
                                                          mate.y,
                                                          mate.h).relativeRobotLocationOf(location))

            if time < minimumTime:
                minimumTime = time
                playerNum = mate.playerNumber

        return playerNum


    ######################################################
    ############       Teammate Stuff     ################
    ######################################################

    def aPrioriTeammateUpdate(self):
        """Here we update information about teammates
        before running a new frame"""

        # update my own information for role switching
        self.time = time.time()
        self.me.updateMe()

        self.goalieIsActive = False
        # loop through teammates
        self.activeFieldPlayers = []
        append = self.activeFieldPlayers.append

        self.numActiveFieldPlayers = 0

        for mate in self.brain.teamMembers:## @TODO!!!! figure out
            #what happened here. We thought we were with another bot
            #when it was in penalty. 7/2011

            # don't check inactive mates
            if mate.active:
                # the goalie isn't a field players
                if mate.isTeammateRole(PBConstants.GOALIE):
                    self.goalieIsActive = True
                else:
                    append(mate)
                    self.numActiveFieldPlayers += 1

    def highestActivePlayerNumber(self, exceptNumbers = []):
        """returns the highest active player number"""
        highNumber = 0
        for mate in self.activeFieldPlayers:
            if mate.playerNumber > highNumber and not mate.playerNumber in exceptNumbers:
                highNumber = mate.playerNumber
        return highNumber

    def lowestActivePlayerNumber(self, exceptNumbers = []):
        """returns the lowest active player number"""
        lowNumber = 10
        for mate in self.activeFieldPlayers:
            if mate.playerNumber < lowNumber and not mate.playerNumber in exceptNumbers:
                lowNumber = mate.playerNumber
        return lowNumber

    def getOtherActiveFieldPlayers(self, exceptNumbers):
        """returns the active teammates who don't have
        a number in exceptNumbers"""

        mates = []
        append = mates.append
        for mate in self.activeFieldPlayers:
            if mate.playerNumber not in exceptNumbers:
                append(mate)
        return mates

    def reset(self):
        """resets all information stored from teammates"""
        for mate in self.brain.teamMembers:
            mate.reset()

    def getForward(self, mates):
        """Gets the robot that is in the most forward position"""
        maxX = 0.0
        forward = self.brain.teamMembers[0]
        for mate in mates:
            mateX = mate.x
            if mateX > maxX:
                maxX = mateX
                forward = mate
        return forward

    def getBack(self, mates):
        """Gets the robot that is in the farthest back position"""
        minX = NogginConstants.FIELD_WIDTH
        back = self.brain.teamMembers[0]
        for mate in mates:
            mateX = mate.x
            if mateX < minX:
                minX = mateX
                back = mate
        return back



    ######################################################
    ############   Strategy Decision Stuff     ###########
    ######################################################

    # Not used - 7/1/11
    def noCalledChaser(self):
        """Returns true if no one is chasing and they are not searching"""
        # If everyone else is out, let's not go for the ball
        #if self.numActiveFieldPlayers == 0:
            #return False

        if (self.brain.gameController.currentState ==
            self.brain.GameController.STATE_READY
            or self.brain.gameController.currentState ==
            self.brain.GameController.STATE_SET):
            return False

        for mate in self.brain.teamMembers:
            if (mate.isTeammateRole(PBConstants.CHASER) or
                mate.isTeammateSubRole(PBConstants.GOALIE_CHASER)):
                return False

        return True

    def useKickoffFormation(self):
        if(self.brain.gameController.timeSincePlaying
           < PBConstants.KICKOFF_FORMATION_TIME):
            return True
        else:
            return False

    def shouldUseDubD(self):
        """
        Uses goalieChaserCount to buffer when we let the goalie call us off.
        If the ball is in our box goalie should be chaser.
        """

        ball = self.brain.ball

        # No matter what state we are we don't
        # Want to become an illegal defender
        # TODO: When ball information is better make this inMyGoalBox
        if ball.x < (NogginConstants.MY_GOALBOX_RIGHT_X + 10):
            self.willBeIllegalD += 1
            if self.willBeIllegalD > PBConstants.DONT_ILLEGAL_D_THRESH:
                self.stopAvoidingBox = 0
                return False    # HACK loc is too broken to do this. we keep being defender.
                return True
        elif ball.vis.on:
            self.stopAvoidingBox += 1
            if self.stopAvoidingBox > PBConstants.STOP_AVOID_BOX_THRESH:
                self.willBeIllegalD = 0

        if not PBConstants.USE_DUB_D:
            return False
        goalie = self.brain.teamMembers[0]
        if goalie.isTeammateSubRole(PBConstants.GOALIE_CHASER):
            self.goalieChaserCount += 1
        else:
            self.goalieChaserCount = 0
            return False
        if self.goalieChaserCount > PBConstants.GOALIE_CHASER_COUNT_THRESH:
            return not self.brain.player.inKickingState

    # Not used as of 6/17/13
    def defenderShouldChase(self):
        ballX = self.brain.ball.rel_x
        goalie = self.brain.teamMembers[0]
        return(ballX < PBConstants.DEFENDER_SHOULD_CHASE_THRESH and
               not goalie.isTeammateSubRole(PBConstants.GOALIE_CHASER) )

    def shouldSwitchSubRole(self, subRoleOnDeck, workingPlay):
        """Returns true if switched into a new role (if time is -1) or
        number of times is greater than threshold or
        if current subrole is the same"""
        if self.subRoleSwitchTime == -1:
            self.subRoleSwitchTime = 0
            return True
        elif workingPlay.isSubRole(subRoleOnDeck):
            self.subRoleSwitchTime = 0
            return True
        else:
            self.subRoleSwitchTime += 1
            if self.subRoleSwitchTime > PBConstants.SUB_ROLE_SWITCH_BUFFER:
                self.subRoleSwitchTime = 0
                return True
        return False



    ######################################################
    ############   Positioning Stuff     #################
    ######################################################

    def getPointBetweenBallAndGoal(self, ball, dist_from_ball):
        """returns defensive position between ball (x,y) and goal (x,y)
        at <dist_from_ball> centimeters away from ball"""
        delta_y = ball.y - NogginConstants.MY_GOALBOX_MIDDLE_Y
        delta_x = ball.x - NogginConstants.MY_GOALBOX_LEFT_X

        # don't divide by 0
        if delta_x == 0:
            delta_x = 0.001
        if delta_y == 0:
            delta_y = 0.001

        pos_x = ball.x - ( dist_from_ball/
                           hypot(delta_x,delta_y) )*delta_x
        pos_y = ball.y - ( dist_from_ball/
                           hypot(delta_x,delta_y) )*delta_y

        return pos_x,pos_y


    # a TODO should be to make this work if we decide to integrate
    # a goalie player back into playbook.
    def fancyGoaliePosition(self):
        """returns a goalie position using ellipse"""

        # lets try maintaining home position until the ball is closer in
        # might help us stay localized better
        ball = self.brain.ball
        h = ball.loc.heading
        position = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

        if ball.dist < PBConstants.ELLIPSE_POSITION_LIMIT:
            # Use an ellipse just above the goalline to determine x and
            # y position. We get the angle from goal center to the ball
            # to determine our X,Y
            theta = atan2( ball.loc.y - PBConstants.LARGE_ELLIPSE_CENTER_Y,
                           ball.loc.x - PBConstants.LARGE_ELLIPSE_CENTER_X)

            thetaDeg = PBConstants.RAD_TO_DEG * theta

            # Clip the angle so that the (x,y)-coordinate is not too
            # close to the posts
            if PBConstants.ELLIPSE_ANGLE_MIN > MyMath.sub180Angle(thetaDeg):
                theta = PBConstants.ELLIPSE_ANGLE_MIN * PBConstants.DEG_TO_RAD
            elif PBConstants.ELLIPSE_ANGLE_MAX < MyMath.sub180Angle(thetaDeg):
                theta = PBConstants.ELLIPSE_ANGLE_MAX * PBConstants.DEG_TO_RAD

        # Determine X,Y of ellipse based on theta, set heading on the ball
            x, y = self.ellipse.getPositionFromTheta(theta)
            position = (x,y,h)

        return position



#############################################################################
#####################     Utility Functions      ############################
#############################################################################

    def printf(self, outputString, printingColor='purple'):
        """FSA print function that allows colors to be specified"""
        if printingColor == 'red':
            print RED_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'blue':
            print BLUE_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'yellow':
            print YELLOW_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'cyan':
            print CYAN_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        elif printingColor == 'purple':
            print PURPLE_COLOR_CODE + str(outputString) + RESET_COLORS_CODE
        else:
            print(str(outputString))


    # Reset counters for role transitions
    # TODO: find a way to make the counters unneeded
    def resetGoalieRoleCounters(self):

        self.shouldStopChaseCounter = 0
        self.shouldChaseCounter = 0
        self.shouldPositionRightCounter = 0
        self.shouldPositionLeftCounter = 0
        self.shouldPositionCenterCounter = 0
        self.shouldSaveCounter = 0
        self.shouldStopSaveCounter = 0


class Ellipse:
  """
  Class to hold information about an ellipse
  """

  def __init__(self, center_x, center_y, semimajorAxis, semiminorAxis):
    self.centerX = center_x
    self.centerY = center_y
    self.a = semimajorAxis
    self.b = semiminorAxis

  def getXfromTheta(self, theta):
    """
    Method to return an X-value on the curve based on angle from center
    Theta is in radians
    """
    return self.a*cos(theta)+self.centerX

  def getYfromTheta(self, theta):
    """
    Method to return a Y-value on the curve based on angle from center
    Theta is in radians
    """
    return self.b*sin(theta)+self.centerY

  def getXfromY(self, y):
    """
    Method to determine the two possible x values based on the y value passed
    """
    return self.getXfromTheta(asin((y-self.centerY)/self.b))

  def getYfromX(self, x):
    """
    Method to determine the two possible y values based on the x value passed
    """
    return self.getYfromTheta(acos((x-self.centerX)/self.a))

  def getPositionFromTheta(self, theta):
      """
      return an (x, y) position from a given angle from the center
      """
      return [self.getXfromTheta(theta), self.getYfromTheta(theta)]
