
from math import (fabs, hypot)

from . import PBDefs
from . import PBConstants
from . import Strategies
from .. import NogginConstants
import time

class GoTeam:
    """
    This is the class which controls all of our coordinated behavior system.  
    Should act as a replacement to the old PlayBook monolith approach
    """
    def __init__(self, brain):
        self.brain = brain
        self.printStateChanges = True

        # Info about all of our states
        # Strategies
        self.currentStrategy = 'sInit'
        self.lastStrategy = 'sInit'
        self.lastDiffStrategy = 'sInit'
        self.strategyCounter = 0
        self.strategyStartTime = 0
        self.strategyTime = 0

        # Formations
        self.currentFormation = 'fInit'
        self.lastFormation = 'fInit'
        self.lastDiffFormation = 'fInit'
        self.formationCounter = 0
        self.formationStartTime = 0
        self.formationTime = 0

        # Roles
        self.currentRole = PBConstants.INIT_ROLE
        self.lastRole = PBConstants.INIT_ROLE
        self.lastDiffRole = PBConstants.INIT_ROLE
        self.roleCounter = 0
        self.roleStartTime = 0
        self.roleTime = 0

        # SubRoles
        self.currentSubRole = PBConstants.INIT_SUB_ROLE
        self.lastSubRole = PBConstants.INIT_SUB_ROLE
        self.lastDiffSubRole = PBConstants.INIT_SUB_ROLE
        self.subRoleCounter = 0
        self.subRoleStartTime = 0
        self.subRoleTime = 0

        self.subRoleOnDeck = PBConstants.INIT_SUB_ROLE
        self.lastSubRoleOnDeck = PBConstants.INIT_SUB_ROLE
        self.subRoleOnDeckCount = 0

        self.subRole = self.currentSubRole
        self.role = self.currentRole
        self.formation = self.currentFormation
        self.strategy = self.currentStrategy

        # Information about teammates
        self.teammates = []
        for i in xrange(PBConstants.NUM_TEAM_PLAYERS):
            mate = PBDefs.Teammate(brain)
            mate.playerNumber = i + 1
            self.teammates.append(mate)
       
        self.position = []
        self.me = self.teammates[self.brain.my.playerNumber - 1]
        self.me.playerNumber = self.brain.my.playerNumber
        self.inactiveMates = []
        self.numInactiveMates = 0
        self.kickoffFormation = 0
        self.timeSinceCaptureChase = 0


    def run(self):
        """
        We run this each frame to get the latest info
        """
        self.aPrioriTeammateUpdate()

        # We will always return a strategy
        self.currentStrategy, self.currentFormation, self.currentRole, \
            self.currentSubRole, self.position = self.strategize()

        # Update all of our new infos
        self.updateStateInfo()
        self.aPosterioriTeammateUpdate()

    def strategize(self):
        """
        Picks the strategy to run and returns all sorts of infos
        """
        # First we check for testing stuff
        if PBConstants.TEST_DEFENDER:
            return Strategies.sTestDefender(self)
        elif PBConstants.TEST_OFFENDER:
            return Strategies.sTestOffender(self)
        elif PBConstants.TEST_MIDDIE:
            return Strategies.sTestMiddie(self)
        elif PBConstants.TEST_CHASER:
            return Strategies.sTestChaser(self)

        # Now we look at shorthanded strategies
        elif self.numInactiveMates == 1:
            return Strategies.sOneDown(self)

        # Here we have the strategy stuff
        return Strategies.sSpread(self)

    def updateStateInfo(self):
        """
        Update information specific to the coordinated behaviors
        """
        # Update our counters!
        # Update Strategy Memory
        if self.lastStrategy != self.currentStrategy:
            if self.printStateChanges:
                self.printf("Strategy switched to "+
                            self.currentStrategy)
            self.strategyCounter = 0
            self.lastDiffStrategy = self.lastStrategy
            self.strategyStartTime = self.getTime()
            self.strategyTime = 0
        else:
            self.strategyCounter += 1
            self.strategyTime = self.getTime() - self.strategyStartTime

        # Update Formations memory
        if self.lastFormation != self.currentFormation:
            if self.printStateChanges:
                self.printf("Formation switched to "+
                            self.currentFormation)
            self.formationCounter = 0
            self.lastDiffFormation = self.lastFormation
            self.formationStartTime = self.getTime()
            self.formationTime = 0
        else:
            self.formationCounter += 1
            self.formationTime = self.getTime() - self.formationStartTime

        # Update memory of roles
        if self.lastRole != self.currentRole:
            if self.printStateChanges:
                self.printf("Role switched to "+
                            PBConstants.ROLES[self.currentRole])
            self.roleCounter = 0
            self.lastDiffRole = self.lastRole
            self.roleStartTime = self.getTime()
            self.roleTime = 0
            self.subRoleOnDeck = self.currentSubRole
            self.subRoleOnDeckCounter = 0
        else:
            self.roleCounter += 1
            self.roleTime = self.getTime() - self.roleStartTime

        # We buffer the subRole switching by making sure that a subrole is 
        # returned for multiple frames in a row
        if (self.subRoleOnDeck and 
            self.subRoleOnDeck != self.currentSubRole):
            # if the sub role on deck is the same as the last, increment
            if self.subRoleOnDeck == self.lastSubRoleOnDeck:
                self.subRoleOnDeckCounter += 1
            # otherwise our idea of subRole has changed, null counter
            else:
                self.subRoleOnDeckCounter = 0

            # if we are sure that we want to switch into this sub role
            if (self.subRoleOnDeckCounter > 
                PbConstants.SUB_ROLE_SWITCH_BUFFER):
                #self.currentSubRole = self.subRoleOnDeck
                self.subRoleOnDeckCounter = 0

        # SubRoles
        if self.lastSubRole != self.currentSubRole:
            if self.printStateChanges:
                try:
                    self.printf("SubRole switched to "+
                                PBConstants.SUB_ROLES[self.currentSubRole])
                except:
                    raise ValueError("Incorrect currentSubRole:" + 
                                     str(self.currentSubRole))
            self.roleCounter = 0
            self.lastDiffSubRole = self.lastSubRole
            self.subRoleStartTime = self.getTime()
            self.subRoleTime = 0
        else:
            self.subRoleCounter += 1
            self.subRoleTime = self.getTime() - self.subRoleStartTime

        # Get ready for the next frame
        self.lastStrategy = self.currentStrategy
        self.lastFormation = self.currentFormation
        self.lastRole = self.currentRole
        self.lastSubRole = self.currentSubRole
        self.lastSubRoleOnDeck = self.subRoleOnDeck

        # Robots are funnnn.... :(
        self.strategy = self.currentStrategy
        self.formation = self.currentFormation
        self.role = self.currentRole
        self.subRole = self.currentSubRole

    def getTime(self):
        return time.time()

    def printf(self, string):
        print string

    def getAction(self):
        '''called by player behaviors. returns a list [x,y,h] for position and
        a role to play'''
        if self.isGoalie():
            self.aPrioriTeammateUpdate()
            return self.noCalledChaser()

        self.run()

        return list(self.position), self.currentStrategie, \
            self.currentFormation, self.currentRole, self.currentSubRole

    ######################################################
    ############       Role Switching Stuff     ##########
    ######################################################
    def determineChaser(self):
        '''return the player number of the chaser'''

        chaser_mate = self.teammates[self.brain.my.playerNumber-1]

        if PBConstants.DEBUG_DET_CHASER:
            print "chaser det: me == #", self.brain.my.playerNumber
        # scroll through the teammates
        for i, mate in enumerate(self.teammates):
            if PBConstants.DEBUG_DET_CHASER:
                print "\t mate #", i+1,
            # TEAMMATE SANITY CHECKS
            # don't check goalie out
            # don't check out penalized or turned-off dogs
            if (mate.playerNumber == PBConstants.GOALIE_NUMBER or  
                mate.inactive or 
                mate.playerNumber == self.brain.my.playerNumber or 
                fabs(mate.ballY - self.brain.ball.y) > 150.0):
#                 (0.0 < self.brain.ball.dist <= 125.0 and                 
#                  fabs(self.brain.ball.dist - 
#                       self.brain.getPosDist([mate.ballX, mate.ballY]) > 
#                       125.0))): 
                if PBConstants.DEBUG_DET_CHASER:
                    print "\t inactive or goalie or me"
                continue

            # if the player has got the ball, return his number
            if (mate.grabbing or
                mate.dribbling or
                mate.kicking):
                if DEBUG_DET_CHASER:
                    print "\t grabbing"
                return mate

            # Calculate chase time locally
            #mate.chaseTime = self.getChaseTime(mate)
            """
            # Tie break stuff
            if self.me.chaseTime < mate.chaseTime:
                chaseTimeScale = self.me.chaseTime
            else:
                chaseTimeScale = mate.chaseTime

            if ((self.me.chaseTime - mate.chaseTime < 
                 CALL_OFF_THRESH + 
                 .15 *chaseTimeScale or
                (self.me.chaseTime - mate.chaseTime < 
                 STOP_CALLING_THRESH +
                 .35 * chaseTimeScale and
                 self.lastRole == CHASER)) and
                mate.playerNumber < self.me.playerNumber):
                if DEBUG_DET_CHASER:
                    print ("\t #%d @ %g >= #%d @ %g" % 
                           (mate.playerNumber, mate.chaseTime, 
                            chaser_mate.playerNumber, 
                            chaser_mate.chaseTime))

                continue
            elif (mate.playerNumber > self.me.playerNumber and
                  mate.chaseTime - self.me.chaseTime < 
                  LISTEN_THRESH + 
                  .45 * chaseTimeScale and
                  mate.calledRole == CHASER):
                chaser_mate = mate

            # else pick the lowest chaseTime
            else:
            if mate.chaseTime < chaser_mate.chaseTime:
                chaser_mate = mate
            """
            if ((mate.ballDist > 0 and chaser_mate.ballDist > 0) and
                (mate.ballDist < chaser_mate.ballDist)):
                chaser_mate = mate 
            elif mate.ballLocDist < chaser_mate.ballLocDist:
                chaser_mate = mate
            if PBConstants.DEBUG_DET_CHASER:
                print ("\t #%d @ %g >= #%d @ %g" % 
                       (mate.playerNumber, mate.ballDist, 
                        chaser_mate.playerNumber, 
                        chaser_mate.ballDist))

        if PBConstants.DEBUG_DET_CHASER:
            print "\t ---- MATE %g WINS" % (chaser_mate.playerNumber)
        # returns teammate instance (could be mine)
        return chaser_mate
    
    def getChaseTime(self, mate):
        '''
        returns estimated time to get to the ball on a chase
        '''
        
        time = 0.0
        # add chase forward time
        time += ((mate.ballLocDist / PBConstants.CHASE_SPEED) *
                 PBConstants.SEC_TO_MILLIS)
        # add bearing correction time
        time += ((fabs(mate.ballLocBearing) / PBConstants.CHASE_SPIN) *
                 PBConstants.SEC_TO_MILLIS)

        # velocity bonus
        if (PBConstants.VB_MIN_REL_VEL_Y < self.brain.ball.relVelY <
            PBConstants.VB_MAX_REL_VEL_Y and
            0 < self.brain.ball.relY < VB_MAX_REL_Y and
            fabs(self.brain.ball.relX + self.brain.ball.relVelX*
                 (self.brain.ball.relY / self.brain.ball.relVelY)) <
            PBConstants.VB_X_THRESH):
            time -= PBConstants.VELOCITY_BONUS

        # ball bearing line to goal bonus
        if mate.y - self.brain.ball.y != 0:
            xDiff = fabs( (mate.x - self.brain.ball.x) /
                          (mate.y - self.brain.ball.y) *
                          (GOAL_BACK_Y - mate.y) + mate.x -
                          GOAL_BACK_X)
        else:
            xDiff = 0

        if ( 0. < xDiff < PBConstants.BEARING_SMOOTHNESS and
             mate.y < self.brain.ball.y):
            time -= (-PBConstants.BEARING_BONUS/
                      PBConstants.BEARING_SMOOTHNESS * 
                      xDiff + PBConstants.BEARING_BONUS)
        elif 0. < xDiff < PBConstants.BEARING_SMOOTHNESS:
            time += (-PBConstants.BEARING_BONUS/
                      PBConstants.BEARING_SMOOTHNESS * 
                      xDiff + PBConstants.BEARING_BONUS)

        # frames off bonus
        if self.brain.ball.framesOff < 3:
            time -= PBConstants.BALL_NOT_SUPER_OFF_BONUS

        return time

    def determineSupporter(self, otherMates, roleInfo):
        '''
        return a player object of the closest teammate in otherMates to
        supportingPos
        '''
        supportRole = roleInfo[0]
        supportingPos = self.position[:]
        supporterMate = self.me
        self.me.supportTime = self.getSupportTime(self.me, supportingPos)

        # scroll through the teammates
        for i, mate in enumerate(otherMates):
            mate.supportTime = self.getSupportTime(mate, supportingPos)

            # Tie break stuff
            supportTimeScale = min(self.me.supportTime, mate.supportTime)

            if ((self.me.supportTime - mate.supportTime <
                 PBConstants.CALL_OFF_THRESH +
                 .15 *supportTimeScale or
                (self.me.supportTime - mate.supportTime <
                 PBConstants.STOP_CALLING_THRESH +
                 .35 * supportTimeScale and
                 self.lastRole == supportRole)) and
                mate.playerNumber < self.me.playerNumber):
                if PBConstants.DEBUG_DET_SUPPORTER:
                    print ("\t #%d @ %g >= #%d @ %g" % 
                           (mate.playerNumber, mate.supportTime,
                            supporterMate.playerNumber,
                            supporterMate.supportTime))

                continue
            elif (mate.playerNumber > self.me.playerNumber and
                  mate.supportTime - self.me.supportTime <
                  PBConstants.LISTEN_THRESH +
                  .45 * supportTimeScale and
                  mate.calledRole == PBConstants.CHASER):
                supporterMate = mate

            # else pick the lowest supportTime
            else:
                if mate.supportTime < supporterMate.supportTime:
                    supporterMate = mate

            if PBConstants.DEBUG_DET_SUPPORTER:
                print ("\t #%d @ %g >= #%d @ %g" %
                       (mate.playerNumber, mate.supportTime,
                        supporterMate.playerNumber,
                        supporterMate.supportTime))

        # returns teammate instance (could be mine)
        return supporterMate

    def getSupportTime(self, mate, supporterSpot):
        '''returns estimated time to get to the supporter spot'''
        
        time = 0.0
        
        positionDist = self.brain.getDist(mate.x, mate.y, supporterSpot[0],
                                          supporterSpot[1])
        positionBearing = fabs(self.brain.getOthersRelativeBearing(
                mate.x, mate.y, mate.h, supporterSpot[0], supporterSpot[1]))

        # Not totally correct, since we move ortho...
        # add forward time
        time += ((positionDist / PBConstants.SUPPORT_SPEED) *
                 PBConstants.SEC_TO_MILLIS)

        # add bearing correction time
        time += ((positionBearing / PBConstants.SUPPORT_SPIN) *
                 PBConstants.SEC_TO_MILLIS)

        return time

    def getLeastWeightPosition(self,positions, mates = None):
        """
        Gets the position for the dog such that the distance all dogs have
        to move is the least possible
        """
        # if there is only one position return the position
        if len(positions) == 1:
            return positions[0]

        # if we have two positions only two possibilites of positions
        elif len(positions) == 2:
            myDist1 = hypot(positions[0][0] - self.brain.my.x,
                            positions[0][1] - self.brain.my.y)
            myDist2 = hypot(positions[1][0] - self.brain.my.x,
                            positions[1][1] - self.brain.my.y)
            mateDist1 = hypot(positions[0][0] - mates.x,
                              positions[0][1] - mates.y)
            mateDist2 = hypot(positions[1][0] - mates.x,
                              positions[1][1] - mates.y)

            if myDist1 + mateDist2 < myDist2 + mateDist1:
                return positions[0]
            else:
                return positions[1]

        # We have three positions, 6 possibilities
        else:
            # Pseudo 2d array to hold the 3 dogs dists to the 3 positions
            dog2, dog3, dog4 = [], [], []
            dog_distances = [dog2,dog3,dog4]
            mates.append(self.teammates[self.me.playerNumber - 1])

            # get distances from every dog every possible position
            for i,dog in enumerate(dog_distances):
                dog_number = mates[i].playerNumber
                x, y = 0.0, 0.0
                # use either my estimate or teammates'
                if dog_number == self.me.playerNumber:
                    x = self.brain.my.x
                    y = self.brain.my.y
                else:
                    x = self.teammates[dog_number-1].x
                    y = self.teammates[dog_number-1].y

                for p in positions:
                    dog.append([hypot(p[0] - x, p[1] - y), p])

            # 6 possible weights and positions
            distances = [ (dog2[0][0] + dog3[1][0] + dog4[2][0],
                           dog2[0][1],  dog3[1][1],  dog4[2][1]),
                          (dog2[1][0] + dog3[2][0] + dog4[0][0],
                           dog2[1][1],  dog3[2][1],  dog4[0][1]),
                          (dog2[2][0] + dog3[0][0] + dog4[1][0],
                           dog2[2][1],  dog3[0][1],  dog4[1][1]),
                          (dog2[1][0] + dog3[0][0] + dog4[2][0],
                           dog2[1][1],  dog3[0][1],  dog4[2][1]),
                          (dog2[2][0] + dog3[1][0] + dog4[0][0],
                           dog2[2][1],  dog3[1][1],  dog4[0][1]),
                          (dog2[0][0] + dog3[2][0] + dog4[1][0],
                           dog2[0][1],  dog3[2][1],  dog4[1][1])]

            # We must find the least weight choice from the 6 possibilities
            min_dist = NogginConstants.FIELD_HEIGHT*3
            chosenPositions = None
            for i,d in enumerate(distances):
                if d[0] < min_dist:
                    min_dist = d[0]
                    chosenPositions = d

            return chosenPositions


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
        self.me.updateMe()
        self.me.chaseTime = self.getChaseTime(self.me)

        # loop through teammates    
        for mate in self.teammates:
            if (self.isTeammateDead(mate)):# or self.isTeammatePenalized(mate)):
                mate.inactive = True
            if (mate.ballDist > 0):
                self.brain.ball.reportBallSeen()

        self.inactiveMates = self.getInactiveFieldPlayers()
        self.numInactiveMates = len(self.inactiveMates)
  
    def  aPosterioriTeammateUpdate(self):
        """
        Here are updates to teammates which occur after running before 
        exiting the frame
        """
        pass

    def isTeammatePenalized(self,teammate):
        '''
        this checks GameController to see if a player is penalized.
        this check is more redundant than anything, because our players stop
        sending packets when they are penalized, so they will most likely
        fall under the isTeammateDead() check anyways.
        '''
        return (
            self.brain.gameController.gc.team.players[
                teammate.playerNumber-1
                ].penalty
            )

    def isTeammateDead(self,teammate):
        '''
        returns True if teammates' last timestamp is sufficiently behind ours.
        however, the dog could still be on but sending really laggy packets.
        '''
        return (teammate.playerNumber != self.brain.my.playerNumber and
                teammate.lastPacketTime == 0 or 
                teammate.lastPacketTime < (time.time() -
                                           PBConstants.PACKET_DEAD_PERIOD))

    def getInactiveFieldPlayers(self):
        '''cycles through teammate objects and returns number of teammates
        that are 'dead'. ignores myself'''
        inactive_teammates = []
        for i,mate in enumerate(self.teammates):
            """"if (mate.inactive and mate.playerNumber != 
                self.brain.my.playerNumber and 
                mate.playerNumber != PBConstants.GOALIE_NUMBER):
                inactive_teammates.append(mate)"""
            if ((time.time() - mate.timeStamp > PBConstants.INACTIVE_THRESH) and
                self.brain.my.playerNumber != mate.playerNumber and
                mate.playerNumber != PBConstants.GOALIE_NUMBER):
                mate.inactive = True
                inactive_teammates.append(mate)
        return inactive_teammates

    def highestActivePlayerNumber(self):
        '''returns true if the player is the highest active player number'''
        activeMates = self.getOtherActiveTeammates()
        
        for mate in activeMates:
            if mate.playerNumber > self.me.playerNumber:
                return False

        return True


    def getNumPenalizedOpponents(self):
        '''returns number of penalized robots on opponents team'''
        return self.brain.gameController.theirTeam.numPenalized

    def teammateHasBall(self):
        '''returns True if any mate has the ball'''
        for i,mate in enumerate(self.teammates):
            if (mate.inactive or 
                mate.playerNumber == self.me.playerNumber):
                continue
            elif (mate.grabbing or 
                  mate.dribbling or 
                  mate.kicking):
                return True
        return False

    def isGoalieInactive(self):
        '''
        Determines if the goalie is dead or penalized
        '''
        return (self.isTeammateDead(self.teammates[0]) or
                #self.isTeammatePenalized(self.teammates[0]) or 
                self.teammates[0].inactive)

    def isGoalie(self):
        return self.me.playernumber == 1
    ######################################################
    ############   Strategy Decision Stuff     ###########
    ######################################################
    def shouldUseDubD(self):
        return ((self.brain.ball.x > NogginConstants.MY_GOALBOX_LEFT_X + 5. and
                 self.brain.ball.x < NogginConstants.MY_GOALBOX_RIGHT_X -5.  and
                 self.brain.ball.y < NogginConstants.MY_GOALBOX_TOP_Y - 5.) or
                (self.brain.ball.x > NogginConstants.MY_GOALBOX_LEFT_X -5. and
                 self.brain.ball.x < NogginConstants.MY_GOALBOX_RIGHT_X + 5. and
                 self.brain.ball.y < NogginConstants.MY_GOALBOX_TOP_Y + 5. and
                 self.teammates[0].calledRole == PBConstants.CHASER))

    def ballInMyGoalBox(self):
        '''
        returns True if estimate of ball (x,y) lies in my goal box
        -includes all y values below top of goalbox 
        (so inside the goal is included)
        '''
        return (self.brain.ball.x > NogginConstants.MY_GOALBOX_LEFT_X and
                self.brain.ball.x < NogginConstants.MY_GOALBOX_RIGHT_X and
                self.brain.ball.y < NogginConstants.MY_GOALBOX_TOP_Y)
    
    def ballNearSideline(self):
        '''
        returns True if ball (x,y) is near any sideline. 
        avoids either goal box
        '''
        return (
            # if BALL is nearing left sideline
            (self.brain.ball.x < NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X + 
             PBConstants.NEAR_LINE_THRESH) or 
            # if ball is nearing right sideline
            (self.brain.ball.x > NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X -
             PBConstants.NEAR_LINE_THRESH) or
            # if ball is left of goalbox AND
            (self.brain.ball.x < NogginConstants.GOALBOX_LEFT_X and 
             # ball is nearing top sideline OR
             ((self.brain.ball.y > NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y - 
               PBConstants.NEAR_LINE_THRESH) or 
              # ball is nearing bottom sideline
              (self.brain.ball.y < NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
               PBConstants.NEAR_LINE_THRESH))) or
            # if ball is right of goalbox AND
            (self.brain.ball.x > NogginConstants.GOALBOX_RIGHT_X and 
             # ball is nearing top sideline OR
             ((self.brain.ball.y > NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y - 
               PBConstants.NEAR_LINE_THRESH) or 
              # ball is nearing bottom sideline
              (self.brain.ball.y < NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
               PBConstants.NEAR_LINE_THRESH)))
            )

    def getPointBetweenBallAndGoal(self,dist_from_ball):
        '''returns defensive position between ball (x,y) and goal (x,y)
        at <dist_from_ball> centimeters away from ball'''
        delta_x = self.brain.ball.x - NogginConstants.MY_GOAL_X
        delta_y = self.brain.ball.y - NogginConstants.MY_GOAL_BOTTOM_Y
        
        pos_x = self.brain.ball.x - (dist_from_ball/
                                     hypot(delta_x,delta_y))*delta_x
        pos_y = self.brain.ball.y - (dist_from_ball/
                                     hypot(delta_x,delta_y))*delta_y
        if pos_y > PBConstants.DEFENSIVE_MIDFIELD_Y:
            pos_y = PBConstants.DEFENSIVE_MIDFIELD_Y
            pos_x = (NogginConstants.MY_GOAL_X + delta_x / delta_y *
                     (PBConstants.DEFENSIVE_MIDFIELD_Y - 
                      NogginConstants.MY_GOAL_BOTTOM_Y))

        return pos_x,pos_y

    def getNonChaserTeammates(self,chaser_mate):
        '''returns non-chaser teammate with you (assuming three dogs)'''
        nonChasers = []

        for elem in self.teammates:
            if ( elem.playerNumber != PBConstants.GOALIE_NUMBER and 
                 elem != chaser_mate and 
                 elem.playerNumber != self.me.playerNumber):
                nonChasers.append(elem)

        return nonChasers

    def getNonChaserPlayers(self, chaser_mate):
        '''
        figure out who the other active teammates are
        '''
        mates = []
        for mate in self.teammates:
            if (not mate.inactive and mate.playerNumber != 
                PBConstants.GOALIE_NUMBER 
                and mate != chaser_mate):
                mates.append(mate)
        return mates

    def getOtherActiveTeammate(self):
        '''this returns the teammate instance of an active teammate that isn't 
        you. THIS ASSUMES THAT THERE IS ALREADY ONE FIELD PLAYER DEAD'''
        # Figure out who isn't penalized with you
        others = [mate for mate in self.teammates if
                  mate.playerNumber != PBConstants.GOALIE_NUMBER  and
                  mate.playerNumber != self.me.playerNumber and
                  not mate.inactive]
        if others:
            return others[0]
        else:
            return None

    def getOtherActiveTeammates(self):
        '''
        figure out who the other active teammates are
        '''
        mates = []
        for mate in self.teammates:
            if (not mate.inactive and mate.playerNumber != 
                PBConstants.GOALIE_NUMBER
                and mate.playerNumber != self.me.playerNumber):
                mates.append(mate)
        return mates

    def noCalledChaser(self):
        """
        Returns true if no one is chasing and they are not searching
        """
        # If everyone else is out, let's not go for the ball
        if len(self.getInactiveFieldPlayers()) == \
                NogginConstants.NUM_TEAM_PLAYERS - 1.:
            return False

        for mate in self.teammates:
            if (not mate.inactive and (mate.calledRole == PBConstants.CHASER
                                       or mate.calledRole == 
                                       PBConstants.SEARCHER)):
                return False
        return True        

    def getGoalDifferential(self):
        '''
        Returns the current goal differential
        '''
        """
        return (self.brain.gameController.gc.teams(0)[0] - 
                self.brain.gameController.gc.teams(1)[1][1])
        """
        return 0
        
    def closeToReadySpot(self, mate):
        """
        Determine if a mate is close to the correct ready spot
        """
        return False
    def reset(self):
        '''resets all information stored from teammates'''
        for i,mate in enumerate(self.teammates):
            mate.reset()


    def update(self,packet):
        '''public method called by Brain.py to update a teammates' info
        with a new packet'''
        self.teammates[packet.playerNumber-1].update(packet)


