from math import (fabs, hypot, cos, sin, acos, asin)
from ..util.MyMath import safe_atan2


from . import Teammate
from . import PBConstants
from . import Strategies
from . import Play
from .. import NogginConstants
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
    """
    This is the class which controls all of our coordinated behavior system.
    Should act as a replacement to the old PlayBook monolith approach
    """
    def __init__(self, brain):
        self.brain = brain
        self.printStateChanges = True

        self.time = time.time()

        self.play = Play.Play()
        self.lastPlay = Play.Play()

        # Information about teammates
        self.teammates = []
        for i in xrange(PBConstants.NUM_TEAM_PLAYERS):
            mate = Teammate.Teammate(brain)
            mate.playerNumber = i + 1
            self.teammates.append(mate)

        self.position = []
        self.me = self.teammates[self.brain.my.playerNumber - 1]
        self.me.playerNumber = self.brain.my.playerNumber
        self.activeFieldPlayers = []
        self.numActiveFieldPlayers = 0
        self.kickoffFormation = 0
        self.timeSinceCaptureChase = 0
        self.pulledGoalie = False
        self.ellipse = Ellipse(PBConstants.LARGE_ELLIPSE_CENTER_X,
                               PBConstants.LARGE_ELLIPSE_CENTER_Y,
                               PBConstants.LARGE_ELLIPSE_HEIGHT,
                               PBConstants.LARGE_ELLIPSE_WIDTH)

    def run(self):
        """
        We run this each frame to get the latest info
        """
        if self.brain.gameController.currentState != 'gamePenalized':
            self.aPrioriTeammateUpdate()
        self.workingPlay = self.strategize()
        # Update all of our new infos
        self.updateStateInfo()

    def strategize(self):
        """
        creates a play, picks the strategy to run, returns the play after
        it is modified by Strategies
        """
        newPlay = Play.Play()
        # We don't control anything in initial or finished
        if self.brain.gameController.currentState == 'gameInitial' or\
            self.brain.gameController.currentState == 'gameFinished':
            #a new play is always initialized to INIT values so there is no need
            #to change them here
            pass

        # Have a separate strategy to easily deal with being penalized
        elif self.brain.gameController.currentState == 'gamePenalized':
            newPlay.setFormation(PBConstants.PENALTY_FORMATION)
            newPlay.setRole(PBConstants.PENALTY_ROLE)
            newPlay.setSubRole(PBConstants.PENALTY_SUB_ROLE)

        # Check for testing stuff
        elif PBConstants.TEST_DEFENDER:
            Strategies.sTestDefender(self, newPlay)
        elif PBConstants.TEST_OFFENDER:
            Strategies.sTestOffender(self, newPlay)
        elif PBConstants.TEST_CHASER:
            Strategies.sTestChaser(self, newPlay)

        # Have a separate ready section to make things simpler
        elif (self.brain.gameController.currentState == 'gameReady' or
              self.brain.gameController.currentState =='gameSet'):
            Strategies.sReady(self, newPlay)

        # Now we look at game strategies
        elif self.numActiveFieldPlayers == 0:
            Strategies.sNoFieldPlayers(self, newPlay)
        elif self.numActiveFieldPlayers == 1:
            Strategies.sOneField(self, newPlay)

        # This is the important area, what is usually used during play
        elif self.numActiveFieldPlayers == 2:
            Strategies.sWin(self, newPlay)

        # This can only be used right now if the goalie is pulled
        elif self.numActiveFieldPlayers == 3:
            Strategies.sThreeField(self, newPlay)

        return newPlay

    def updateStateInfo(self):
        """
        Update information specific to the coordinated behaviors
        """
        # Update Play Memory
        self.lastPlay = self.play
        self.play = self.workingPlay

        #update teammate instance of self
        self.me.play = self.play

        if not self.play.equals(self.lastPlay):
            if self.printStateChanges:
                self.printf("Play switched to " + self.play.__str__())

    ######################################################
    ############       Role Switching Stuff     ##########
    ######################################################
    def determineChaser(self):
        '''return the player number of the chaser'''

        chaser_mate = self.me

        if PBConstants.DEBUG_DET_CHASER:
            self.printf("chaser det: me == #%g"% self.brain.my.playerNumber)

        #save processing time and skip the rest if we have the ball
        if self.me.hasBall(): #and self.me.isChaser()?
            if PBConstants.DEBUG_DET_CHASER:
                self.printf("I have the ball")
            return chaser_mate

        # scroll through the teammates
        for mate in self.activeFieldPlayers:
            if PBConstants.DEBUG_DET_CHASER:
                self.printf("\t mate #%g"% mate.playerNumber)

            # If the player number is me, or our ball models are super divergent ignore
            if (mate.playerNumber == self.me.playerNumber or
                fabs(mate.ballY - self.brain.ball.y) >
                PBConstants.BALL_DIVERGENCE_THRESH or
                fabs(mate.ballX - self.brain.ball.x) >
                PBConstants.BALL_DIVERGENCE_THRESH):

                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("Ball models are divergent, or it's me")
                continue
            #dangerous- two players might both have ball, both would stay chaser
            #same as the aibo code but thresholds for hasBall are higher now
            elif mate.hasBall():
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("mate %g has ball" % mate.playerNumber)
                chaser_mate = mate
            else:
                # Tie break stuff
                if self.me.chaseTime < mate.chaseTime:
                    chaseTimeScale = self.me.chaseTime
                else:
                    chaseTimeScale = mate.chaseTime
                #TO-DO: break into a separate function call
                if ((self.me.chaseTime - mate.chaseTime <
                     PBConstants.CALL_OFF_THRESH + .15 *chaseTimeScale or
                     (self.me.chaseTime - mate.chaseTime <
                      PBConstants.STOP_CALLING_THRESH + .35 * chaseTimeScale and
                      self.me.isTeammateRole(PBConstants.CHASER))) and
                    mate.playerNumber < self.me.playerNumber):
                    if PBConstants.DEBUG_DET_CHASER:
                        self.printf("\t #%d @ %g >= #%d @ %g" %
                               (mate.playerNumber, mate.chaseTime,
                                chaser_mate.playerNumber,
                                chaser_mate.chaseTime))
                        continue

                elif (mate.playerNumber > self.me.playerNumber and
                      mate.chaseTime - self.me.chaseTime <
                      PBConstants.LISTEN_THRESH + .45 * chaseTimeScale and
                      mate.isChaser()):
                    chaser_mate = mate

                # else pick the lowest chaseTime
                else:
                    if mate.chaseTime < chaser_mate.chaseTime:
                        chaser_mate = mate

                    if PBConstants.DEBUG_DET_CHASER:
                        self.printf (("\t #%d @ %g >= #%d @ %g" %
                                      (mate.playerNumber, mate.chaseTime,
                                       chaser_mate.playerNumber,
                                       chaser_mate.chaseTime)))

        if PBConstants.DEBUG_DET_CHASER:
            self.printf ("\t ---- MATE %g WINS" % (chaser_mate.playerNumber))
        # returns teammate instance (could be mine)
        return chaser_mate

    def getLeastWeightPosition(self,positions, mates = None):
        """
        Gets the position for the robot such that the distance all robot have
        to move is the least possible
        """
        # if there is only one position return the position
        if len(positions) == 1 or mates == None:
            return positions[0]

        # if we have two positions only two possibilites of positions
        #TODO: tie-breaking?
        elif len(positions) == 2:
            myDist1 = hypot(positions[0][0] - self.brain.my.x,
                            positions[0][1] - self.brain.my.y)
            myDist2 = hypot(positions[1][0] - self.brain.my.x,
                            positions[1][1] - self.brain.my.y)
            mateDist1 = hypot(positions[0][0] - mates.x,
                              positions[0][1] - mates.y)
            mateDist2 = hypot(positions[1][0] - mates.x,
                              positions[1][1] - mates.y)

            if myDist1 + mateDist2 == myDist2 + mateDist1:
                if self.me.playerNumber > mates.playerNumber:
                    return positions[0]
                else:
                    return positions[1]
            elif myDist1 + mateDist2 < myDist2 + mateDist1:
                return positions[0]
            else:
                return positions[1]

    ######################################################
    ############       Teammate Stuff     ################
    ######################################################

    def aPrioriTeammateUpdate(self):
        '''
        Here we update information about teammates before running a new frame
        '''
        # Change which wing is forward based on the opponents score
        # self.kickoffFormation = (self.brain.gameController.theirTeam.teamScore) % 2

        # update my own information for role switching
        self.time = time.time()
        self.me.updateMe()
        self.pulledGoalie = self.pullTheGoalie()
        # loop through teammates
        self.activeFieldPlayers = []
        append = self.activeFieldPlayers.append

        self.numActiveFieldPlayers = 0
        for mate in self.teammates:
            if (mate.isPenalized() or mate.isDead()):
                #reset to false when we get a new packet from mate
                mate.active = False
            elif (mate.active and (not mate.isTeammateRole(PBConstants.GOALIE)
                                   or (mate.isDefaultGoalie() and self.pulledGoalie))):
                append(mate)
                self.numActiveFieldPlayers += 1

                # Not using teammate ball reports for now
                if (mate.ballDist > 0 and PBConstants.USE_FINDER):
                    self.brain.ball.reportBallSeen()

    def highestActivePlayerNumber(self):
        '''returns true if the player is the highest active player number'''
        activeMate = self.getOtherActiveTeammate()
        if activeMate.playerNumber > self.me.playerNumber:
            return False
        return True

    def getOtherActiveTeammate(self):
        '''this returns the teammate instance of an active teammate that isn't
        you.'''
        for mate in self.activeFieldPlayers:
            if self.me.playerNumber != mate.playerNumber:
                return mate

    def reset(self):
        '''resets all information stored from teammates'''
        for i,mate in enumerate(self.teammates):
            mate.reset()

    def update(self,packet):
        '''public method called by Brain.py to update a teammates' info
        with a new packet'''
        self.teammates[packet.playerNumber-1].update(packet)

    ######################################################
    ############   Strategy Decision Stuff     ###########
    ######################################################

    def ballInMyGoalBox(self):
        '''
        returns True if estimate of ball (x,y) lies in my goal box
        -includes all y values below top of goalbox
        (so inside the goal is included)
        '''
        return (self.brain.ball.y > NogginConstants.MY_GOALBOX_BOTTOM_Y and
                self.brain.ball.x < NogginConstants.MY_GOALBOX_RIGHT_X and
                self.brain.ball.y < NogginConstants.MY_GOALBOX_TOP_Y)

    def goalieShouldChase(self):
        return self.noCalledChaser()

    def noCalledChaser(self):
        """
        Returns true if no one is chasing and they are not searching
        """
        # If everyone else is out, let's not go for the ball
        #if len(self.getActiveFieldPlayers()) == 0:
            #return False

        if self.brain.gameController.currentState == 'gameReady' or\
                self.brain.gameController.currentState =='gameSet':
            return False

        for mate in self.teammates:
            if (mate.active and (mate.isTeammateRole(PBConstants.CHASER)
                                 or mate.isTeammateRole(PBConstants.SEARCHER))):
                return False
        return True

    def pullTheGoalie(self):
        if PBConstants.PULL_THE_GOALIE:
            if self.brain.gameController.getScoreDifferential() <= -3:
                return True
        return False

################################################################################
#####################     Utility Functions      ###############################
################################################################################

    def printf(self,outputString, printingColor='purple'):
        '''FSA print function that allows colors to be specified'''
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


    def determineChaseTime(self, useZone = False):
        """
        Metric for deciding chaser.
        Attempt to define a time to get to the ball.
        Can give bonuses or penalties in certain situations.
        """
        # if the robot sees the ball use visual distances to ball
        time = 0.0

        if PBConstants.DEBUG_DETERMINE_CHASE_TIME:
            self.printf("DETERMINE CHASE TIME DEBUG")
        if self.me.ballDist > 0:
            time += (self.me.ballDist / PBConstants.CHASE_SPEED) *\
                PBConstants.SEC_TO_MILLIS

            if PBConstants.DEBUG_DETERMINE_CHASE_TIME:
                self.printf("\tChase time base is " + str(time))

            # Give a bonus for seeing the ball
            time -= PBConstants.BALL_ON_BONUS

            if PBConstants.DEBUG_DETERMINE_CHASE_TIME:
                self.printf("\tChase time after ball on bonus " + str(time))

        else: # use loc distances if no visual ball
            time += (self.me.ballLocDist / PBConstants.CHASE_SPEED) *\
                PBConstants.SEC_TO_MILLIS
            if PBConstants.DEBUG_DETERMINE_CHASE_TIME:
                self.printf("\tChase time base is " + str(time))


        # Add a penalty for being fallen over
        time += (self.brain.fallController.getTimeRemainingEst() *
                 PBConstants.SEC_TO_MILLIS)

        if PBConstants.DEBUG_DETERMINE_CHASE_TIME:
            self.printf("\tChase time after fallen over penalty " + str(time))
            self.printf("")

        return time

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
