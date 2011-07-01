from math import (fabs, hypot, atan2, cos, sin, acos, asin)
from ..util import MyMath
from . import PBConstants
from . import Strategies
import noggin_constants as NogginConstants
import time

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
    """This is the class which controls all of our coordinated behavior system.
       Should act as a replacement to the old PlayBook monolith approach"""
    def __init__(self, brain):
        self.brain = brain
        self.printStateChanges = True

        self.time = time.time()

        # Information about teammates

        #self.position = []
        self.me = self.brain.teamMembers[self.brain.my.playerNumber - 1]
        self.me.playerNumber = self.brain.my.playerNumber
        self.activeFieldPlayers = []
        self.numActiveFieldPlayers = 0
        self.kickoffFormation = 0
        self.timeSinceCaptureChase = 0
        self.subRoleSwitchTime = 0
        self.goalieChaserCount = 0
        self.ellipse = Ellipse(PBConstants.LARGE_ELLIPSE_CENTER_X,
                               PBConstants.LARGE_ELLIPSE_CENTER_Y,
                               PBConstants.LARGE_ELLIPSE_HEIGHT,
                               PBConstants.LARGE_ELLIPSE_WIDTH)


        # Goalie
        self.shouldPositionLeftCounter = 0
        self.shouldPositionRightCounter = 0
        self.shouldPositionCenterCounter = 0
        self.shouldSaveCounter = 0
        self.shouldChaseCounter = 0
        self.shouldStopChaseCounter = 0
        self.shouldStopSaveCounter = 0

    def run(self, play):
        """We run this each frame to get the latest info"""
        if self.brain.gameController.currentState != 'gamePenalized':
            self.aPrioriTeammateUpdate()

        if self.brain.gameController.currentState == 'gameReady':
            # Change which wing is forward based on the opponents score
            gc = self.brain.gameController.gc
            self.kickoffFormation = (gc.teams(self.brain.my.teamColor)[1])%2

        play.changed = False
        self.strategize(play)

        # Update all of our new infos
        self.updateStateInfo(play)

    def strategize(self, play):
        """
        creates a play, picks the strategy to run, returns the play after
        it is modified by Strategies
        """
        currentGCState = self.brain.gameController.currentState
        # We don't control anything in initial or finished
        if (currentGCState == 'gameInitial' or
            currentGCState == 'gameFinished'):
            if not play.isSubRole(PBConstants.INIT_SUB_ROLE):
                play.setStrategy(PBConstants.INIT_STRATEGY)
                play.setFormation(PBConstants.INIT_FORMATION)
                play.setRole(PBConstants.INIT_ROLE)
                play.setSubRole(PBConstants.INIT_SUB_ROLE)

        # Have a separate strategy to easily deal with being penalized
        elif currentGCState == 'gamePenalized':
            if not play.isSubRole(PBConstants.PENALTY_SUB_ROLE):
                play.setStrategy(PBConstants.PENALTY_STRATEGY)
                play.setFormation(PBConstants.PENALTY_FORMATION)
                play.setRole(PBConstants.PENALTY_ROLE)
                play.setSubRole(PBConstants.PENALTY_SUB_ROLE)

        # Check for testing stuff
        elif PBConstants.TEST_DEFENDER:
            Strategies.sTestDefender(self, play)
        elif PBConstants.TEST_OFFENDER:
            Strategies.sTestOffender(self, play)
        elif PBConstants.TEST_CHASER:
            Strategies.sTestChaser(self, play)

        # Have a separate ready section to make things simpler
        elif (currentGCState == 'gameReady' or
              currentGCState =='gameSet'):
            Strategies.sReady(self, play)

        # Now we look at game strategies
        elif self.numActiveFieldPlayers == 0:
            Strategies.sNoFieldPlayers(self, play)
        elif self.numActiveFieldPlayers == 1:
            Strategies.sOneField(self, play)
        elif self.numActiveFieldPlayers == 2:
            Strategies.sTwoField(self, play)
        elif self.numActiveFieldPlayers == 3:
            Strategies.sWin(self, play)

    def updateStateInfo(self, play):
        """
        Update information specific to the coordinated behaviors
        """
        # Print changes
        if play.changed:
            self.brain.speech.say(PBConstants.SUB_ROLES[play.subRole])
            if self.printStateChanges:
                self.printf("Play switched to " + play.__str__())

    ######################################################
    ############       Role Switching Stuff     ##########
    ######################################################
    def determineChaser(self, play):
        """return the team member who is the chaser"""
        chaser_mate = self.activeFieldPlayers[0]

        if PBConstants.DEBUG_DET_CHASER:
            self.printf("chaser det: me == #%g"% self.brain.my.playerNumber)

        # save processing time and skip the rest if we have the ball
        if self.brain.player.inKickingState and play.isChaser():
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

            elif mate.hasBall():
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("mate %g has ball" % mate.playerNumber)
                chaser_mate = mate

            else:
                # Tie breaking. Method described in Robust Team Play, by Henry Work
                ## NOTE: Took out chaseTimeScale (which was just the minimum chase-
                ##      time between two robots) because it wansn't guaranteeing that
                ##      the thresholds would be appropriately tiered. A good idea,
                ##      but bad implementation. May work with some futsing around.
                ##      -- Wils (06/24/11)
                if self.shouldCallOff(chaser_mate, mate):
                    if PBConstants.DEBUG_DET_CHASER:
                        self.printf("\t #%d @ %g >= #%d @ %g, shouldCallOff" %
                               (mate.playerNumber, mate.chaseTime,
                                chaser_mate.playerNumber, chaser_mate.chaseTime))
                    if self.shouldListen(chaser_mate, mate):
                        if PBConstants.DEBUG_DET_CHASER:
                            self.printf(("\t #%d @ %g <= #%d @ %g, shouldListen" %
                                         (mate.playerNumber, mate.chaseTime,
                                          chaser_mate.playerNumber,
                                          chaser_mate.chaseTime)))
                        continue

                    chaser_mate = mate

                # else pick the lowest chaseTime
                else:
                    if mate.chaseTime < chaser_mate.chaseTime:
                        chaser_mate = mate
                    if PBConstants.DEBUG_DET_CHASER:
                        self.printf (("\t #%d @ %g >= #%d @ %g, normal comparison" %
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
        return(((mate.chaseTime - chaser_mate.chaseTime) <
                PBConstants.CALL_OFF_THRESH) or
               ((mate.chaseTime - chaser_mate.chaseTime) <
                PBConstants.STOP_CALLING_THRESH and
                mate.isTeammateRole(PBConstants.CHASER)))

    def shouldListen(self, chaser_mate, mate):
        """Decides if mate should listen to the chaser_mate after calling off"""
        # mate = A, chaser_mate = B.
        # A will relinquish chaser to chaser_mate if:
        # chaseTime(B) < chaseTime(A) - m
        # A is higher robot that has decided to be chaser.
        return (chaser_mate.chaseTime < (mate.chaseTime -
                PBConstants.LISTEN_THRESH))

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
            myDist1 = hypot(positions[0].toTupleXY()[0] - self.brain.my.x,
                            positions[0].toTupleXY()[1] - self.brain.my.y)
            myDist2 = hypot(positions[1].toTupleXY()[0] - self.brain.my.x,
                            positions[1].toTupleXY()[1] - self.brain.my.y)
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
                    x = self.brain.my.x
                    y = self.brain.my.y
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

            # chosen Postitions is an array of size 4 where 1,2,3 are the positions
            # returns a Location
            return chosenPositions[self.me.playerNumber -1]



    ######################################################
    ############       Teammate Stuff     ################
    ######################################################

    def aPrioriTeammateUpdate(self):
        """Here we update information about teammates before running a new frame"""
        # update my own information for role switching
        self.time = time.time()
        self.me.updateMe()

        # loop through teammates
        self.activeFieldPlayers = []
        append = self.activeFieldPlayers.append

        self.numActiveFieldPlayers = 0
        for mate in self.brain.teamMembers: ## @TODO!!!! figure out what happened here. We thought we were with another bot when it was in penalty.
            # don't check inactive mates or the goalie.
            if (mate.active and not mate.isTeammateRole(PBConstants.GOALIE)):
                append(mate)
                self.numActiveFieldPlayers += 1

    def highestActivePlayerNumber(self):
        """returns true if the player is the highest active player number"""
        highNumber = 0
        for mate in self.activeFieldPlayers:
            if mate.playerNumber > highNumber:
                highNumber = mate.playerNumber
        return highNumber

    def getOtherActiveFieldPlayers(self, exceptNumbers):
        """returns the active teammates who don't have a number in exceptNumbers"""
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

    def noCalledChaser(self):
        """Returns true if no one is chasing and they are not searching"""
        # If everyone else is out, let's not go for the ball
        #if self.numActiveFieldPlayers == 0:
            #return False

        if self.brain.gameController.currentState == 'gameReady' or\
                self.brain.gameController.currentState =='gameSet':
            return False

        for mate in self.brain.teamMembers:
            if (mate.isTeammateRole(PBConstants.CHASER) or
                mate.isTeammateSubRole(PBConstants.GOALIE_CHASER)):
                return False

        return True

    def useKickoffFormation(self):
        if (self.brain.gameController.timeSincePlay() < \
            PBConstants.KICKOFF_FORMATION_TIME):
            return True
        else:
            return False

    def shouldUseDubD(self):
        """
        Uses goalieChaserCount to buffer when we let the goalie call us off.
        """
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

    def defenderShouldChase(self):
        ballX = self.brain.ball.relX
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

    def fancyGoaliePosition(self):
        """returns a goalie position using ellipse"""

        # lets try maintaining home position until the ball is closer in
        # might help us stay localized better
        ball = self.brain.ball
        h = ball.heading
        position = (PBConstants.GOALIE_HOME_X, PBConstants.GOALIE_HOME_Y, h)

        if ball.dist < PBConstants.ELLIPSE_POSITION_LIMIT:
            # Use an ellipse just above the goalline to determine x and y position
            # We get the angle from goal center to the ball to determine our X,Y
            theta = atan2( ball.y - PBConstants.LARGE_ELLIPSE_CENTER_Y,
                           ball.x - PBConstants.LARGE_ELLIPSE_CENTER_X)

            thetaDeg = PBConstants.RAD_TO_DEG * theta

            # Clip the angle so that the (x,y)-coordinate is not too close to the posts
            if PBConstants.ELLIPSE_ANGLE_MIN > MyMath.sub180Angle(thetaDeg):
                theta = PBConstants.ELLIPSE_ANGLE_MIN * PBConstants.DEG_TO_RAD
            elif PBConstants.ELLIPSE_ANGLE_MAX < MyMath.sub180Angle(thetaDeg):
                theta = PBConstants.ELLIPSE_ANGLE_MAX * PBConstants.DEG_TO_RAD

        # Determine X,Y of ellipse based on theta, set heading on the ball
            x, y = self.ellipse.getPositionFromTheta(theta)
            position = (x,y,h)

        return position



################################################################################
#####################     Utility Functions      ###############################
################################################################################

    def printf(self, outputString, printingColor='purple'):
        """FSA print function that allows colors to be specified"""
        if printingColor == 'red':
            self.brain.out.printf(RED_COLOR_CODE + str(outputString) +\
                                      RESET_COLORS_CODE)
        elif printingColor == 'blue':
            self.brain.out.printf(BLUE_COLOR_CODE + str(outputString) +\
                                      RESET_COLORS_CODE)
        elif printingColor == 'yellow':
            self.brain.out.printf(YELLOW_COLOR_CODE + str(outputString) +\
                                      RESET_COLORS_CODE)
        elif printingColor == 'cyan':
            self.brain.out.printf(CYAN_COLOR_CODE + str(outputString) +\
                                      RESET_COLORS_CODE)
        elif printingColor == 'purple':
            self.brain.out.printf(PURPLE_COLOR_CODE + str(outputString) +\
                                      RESET_COLORS_CODE)
        else:
            self.brain.out.printf(str(outputString))


    # Reset counters for role transitions
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
