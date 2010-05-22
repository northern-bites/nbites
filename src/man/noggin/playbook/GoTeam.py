from math import (hypot, cos, sin, acos, asin)


from . import PBConstants
from . import Strategies
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

        # Information about teammates

        #self.position = []
        self.me = self.brain.teamMembers[self.brain.my.playerNumber - 1]
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

    def run(self, play):
        """
        We run this each frame to get the latest info
        """
        if self.brain.gameController.currentState != 'gamePenalized':
            self.aPrioriTeammateUpdate()

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

        # This is the important area, what is usually used during play
        elif self.numActiveFieldPlayers == 2:
            Strategies.sWin(self, play)

        # This can only be used right now if the goalie is pulled
        elif self.numActiveFieldPlayers == 3:
            Strategies.sThreeField(self, play)

    def updateStateInfo(self, play):
        """
        Update information specific to the coordinated behaviors
        """
        if play.changed:
            if self.printStateChanges:
                self.printf("Play switched to " + play.__str__())

    ######################################################
    ############       Role Switching Stuff     ##########
    ######################################################
    def determineChaser(self):
        """return the player number of the chaser"""

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
            if mate.playerNumber == self.me.playerNumber:
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("it's me")

                continue

            elif mate.hasBall():
                if PBConstants.DEBUG_DET_CHASER:
                    self.printf("mate %g has ball" % mate.playerNumber)
                chaser_mate = mate

            ## elif (fabs(mate.ballY - self.brain.ball.y) >
            ##     PBConstants.BALL_DIVERGENCE_THRESH or
            ##     fabs(mate.ballX - self.brain.ball.x) >
            ##     PBConstants.BALL_DIVERGENCE_THRESH):

            ##     if PBConstants.DEBUG_DET_CHASER:
            ##         self.printf("Ball models are divergent")
            ##     continue

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

                #TO-DO: break into a separate function call
                elif (mate.playerNumber > self.me.playerNumber and
                      mate.chaseTime - self.me.chaseTime <
                      PBConstants.LISTEN_THRESH + .45 * chaseTimeScale and
                      mate.isTeammateRole(PBConstants.CHASER)):
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
        """
        Here we update information about teammates before running a new frame
        """
        # Change which wing is forward based on the opponents score
        # self.kickoffFormation =
        #(self.brain.gameController.theirTeam.teamScore) % 2

        # update my own information for role switching
        self.time = time.time()
        self.me.updateMe()
        self.pulledGoalie = self.pullTheGoalie()
        # loop through teammates
        self.activeFieldPlayers = []
        append = self.activeFieldPlayers.append

        self.numActiveFieldPlayers = 0
        for mate in self.brain.teamMembers:
            if (mate.active and mate.isDead()): #no need to check inactive mates
                #we set to True when we get a new packet from mate
                mate.active = False
            elif (mate.active and (not mate.isTeammateRole(PBConstants.GOALIE)
                                   or (mate.isDefaultGoalie() and self.pulledGoalie))):
                append(mate)
                self.numActiveFieldPlayers += 1

    def highestActivePlayerNumber(self):
        """returns true if the player is the highest active player number"""
        activeMate = self.getOtherActiveTeammate()
        if activeMate.playerNumber > self.me.playerNumber:
            return False
        return True

    def getOtherActiveTeammate(self):
        """this returns the teammate instance of an active teammate that isn't
        you."""
        for mate in self.activeFieldPlayers:
            if self.me.playerNumber != mate.playerNumber:
                return mate

    def reset(self):
        """resets all information stored from teammates"""
        for mate in self.brain.teamMembers:
            mate.reset()

    ######################################################
    ############   Strategy Decision Stuff     ###########
    ######################################################

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

        # TO-DO: switch this to activeFieldPlayers
        for mate in self.activeFieldPlayers:
            if mate.isTeammateRole(PBConstants.CHASER):
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
