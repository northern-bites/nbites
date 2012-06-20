from objects import (RobotLocation, Location)
from math import fabs
import noggin_constants as NogginConstants
import time

OPP_GOAL = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                    NogginConstants.OPP_GOALBOX_MIDDLE_Y)
OPP_GOAL_LEFT_POST = Location(NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                              NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y)
OPP_GOAL_RIGHT_POST = Location(NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_X,
                              NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y)
PACKET_DEAD_PERIOD = 4
DEFAULT_GOALIE_NUMBER = 1
DEFAULT_DEFENDER_NUMBER = 2
DEFAULT_OFFENDER_NUMBER = 3
DEFAULT_CHASER_NUMBER = 4
DEBUG_DETERMINE_CHASE_TIME = False
SEC_TO_MILLIS = 1000.0
CHASE_SPEED = 20.00 #cm/sec
CHASE_TIME_SCALE = 0.45              # How much new measurement is used.
BALL_OFF_PENALTY = 1000.             # Big penalty for not seeing the ball.
                                     # If you change this, modify determineChaser
BALL_GOAL_LINE_PENALTY = 10.

# Behavior constants
BALL_TEAMMATE_DIST_GRABBING = 35
BALL_TEAMMATE_BEARING_GRABBING = 45.
BALL_TEAMMATE_DIST_DRIBBLING = 20

# Ball on?
BALL_FRAMES = 20

class TeamMember(RobotLocation):
    """class for keeping track of teammates' info """

    def __init__(self, tbrain=None):
        '''variables include lots from the Packet class'''
        RobotLocation.__init__(self, 0.0, 0.0, 0.0)
        # things in the Packet()
        self.playerNumber = 0
        self.ballDist = 0
        self.ballBearing = 0
        self.ballX = 0
        self.ballY = 0
        self.ballOn = False
        self.role = None
        self.subRole = None
        self.chaseTime = 0
        self.lastPacketTime = time.time()

        #other info we want stored
        self.brain = tbrain # brain instance
        self.active = True
        self.grabbing = False
        self.dribbling = False

    def update(self, packet):
        '''
        receives a packet, updates teammate information. packets received
        have already been verified by timestamp system, so we can assume
        they are LEGIT.
        '''

        # stores packet information locally
        self.playerNumber = packet.playerNumber
        self.x = packet.playerX
        self.y = packet.playerY
        self.h = packet.playerH
        self.ballDist = packet.ballDist
        self.ballBearing = packet.ballBearing
        self.ballX = packet.ballX
        self.ballY = packet.ballY
        self.ballOn = packet.ballOn
        self.role = packet.role
        self.subRole = packet.subRole
        self.chaseTime = packet.chaseTime

        # calculates ball localization distance, bearing
        self.bearingToGoal = self.getBearingToGoal()
        # if we are recieving packets, teammate is active
        self.active = True
        self.grabbing = (self.ballDist <=
                         BALL_TEAMMATE_DIST_GRABBING) and \
                         (fabs(self.ballBearing) <
                          BALL_TEAMMATE_BEARING_GRABBING)

        #potential problem when goalie is grabbing?
        #only going to be dribbling or grabbing if you see the ball
        self.dribbling = self.ballDist <= \
                          BALL_TEAMMATE_DIST_DRIBBLING

        self.lastPacketTime = time.time()


    def updateMe(self):
        """
        updates my information as a teammate (since we don't get our own
        packets)
        """

        my = self.brain.my
        ball = self.brain.ball

        self.x = my.x
        self.y = my.y
        self.h = my.h
        self.ballDist = ball.loc.dist
        self.ballBearing = ball.loc.bearing
        self.ballX = ball.loc.x
        self.ballY = ball.loc.y
        self.ballOn = ball.vis.on
        self.role = self.brain.play.role
        self.subRole = self.brain.play.subRole
        self.chaseTime = self.determineChaseTime()

        self.active = (not self.brain.gameController.currentState ==
                       'gamePenalized')

        self.dribbling = self.ballDist <= \
                         BALL_TEAMMATE_DIST_DRIBBLING

        self.grabbing = (self.ballDist <=
                         BALL_TEAMMATE_DIST_GRABBING) and \
                         (fabs(self.ballBearing) <
                          BALL_TEAMMATE_BEARING_GRABBING)

        self.lastPacketTime = time.time()

    def reset(self):
        '''Reset all important Teammate variables'''
        self.x = 0
        self.y = 0
        self.h = 0
        self.ballDist = 0
        self.ballBearing = 0
        self.role = None
        self.subRole = None
        self.active = False
        self.grabbing = False
        self.dribbling = False

    def getBearingToGoal(self):
        """returns bearing to goal"""
        return self.getRelativeBearing(OPP_GOAL)

    def determineChaseTime(self):
        """
        Attempt to define a time in seconds to get to the ball.
        Can give penalties in certain situations.
        @ return: returns a time in milliseconds with penalties.
        Note: Don't give bonuses. It can result in negative chase times
              which can screw up the math later on. --Wils (06/25/11)
        """
        t = (self.ballDist / CHASE_SPEED)

        if DEBUG_DETERMINE_CHASE_TIME:
            self.brain.out.printf("\tChase time base is " + str(t))

        # Give a penalty for not seeing the ball if we aren't in a kickingState
        if (not self.brain.ball.vis.framesOn > 3 and
            not self.brain.player.inKickingState):
            t += BALL_OFF_PENALTY

        if DEBUG_DETERMINE_CHASE_TIME:
            self.brain.out.printf("\tChase time after ball on bonus " + str(t))

        # Commented out Summer 2012 due to unreliable Localization.
        # # Give penalties for not lining up along the ball-goal line
        # lpb = self.getRelativeBearing(OPP_GOAL_LEFT_POST) #left post bearing
        # rpb = self.getRelativeBearing(OPP_GOAL_RIGHT_POST) #right post bearing
        # # TODO: scale these by how far off we are??
        # # ball is not lined up
        # if (self.ballBearing > lpb or rpb > self.ballBearing):
        #     t += BALL_GOAL_LINE_PENALTY
        # # we are not lined up
        # if (lpb < 0 or rpb > 0):
        #     t += BALL_GOAL_LINE_PENALTY

        # if DEBUG_DETERMINE_CHASE_TIME:
        #     self.brain.out.printf("\tChase time after ball-goal-line penalty "+str(t))

        # Add a penalty for being fallen over
        t += self.brain.fallController.getTimeRemainingEst()

        if DEBUG_DETERMINE_CHASE_TIME:
            self.brain.out.printf("\tChase time after fallen over penalty " + str(t))

        t *= CHASE_SPEED

        # Filter by IIR to reduce noise
        t = t * CHASE_TIME_SCALE + (1.0 -CHASE_TIME_SCALE) * self.chaseTime

        if DEBUG_DETERMINE_CHASE_TIME:
            self.brain.out.printf("\tChase time after filter " +str(t))
            self.brain.out.printf("")

        return t

    def hasBall(self):
        return self.grabbing

    def isTeammateRole(self, roleToTest):
        return (self.role == roleToTest)

    def isTeammateSubRole(self, subRoleToTest):
        return (self.subRole == subRoleToTest)

    def isDefaultGoalie(self):
        return (self.playerNumber == DEFAULT_GOALIE_NUMBER)

    def isDefaultChaser(self):
        return (self.playerNumber == DEFAULT_CHASER_NUMBER)

    def isDefaultOffender(self):
        return (self.playerNumber == DEFAULT_OFFENDER_NUMBER)

    def isDefaultDefender(self):
        return (self.playerNumber == DEFAULT_DEFENDER_NUMBER)

    def isPenalized(self):
        """
        this checks GameController to see if a player is penalized.
        this check is more redundant than anything, because our players stop
        sending packets when they are penalized, so they will most likely
        fall under the isTeammateDead() check anyways.
        """
        #penalty state is the first item the player tuple [0]
        #penalty state == 1 is penalized
        return (
            self.brain.gameController.gc.players(self.playerNumber)[0]!=0
           )

    def isDead(self):
        """
        returns True if teammates last timestamp is sufficiently behind ours.
        however, the bot could still be on but sending really laggy packets.
        """
        if ((self.brain.time + PACKET_DEAD_PERIOD) < self.lastPacketTime):
            print "\nTIME GOT RESET! AH!!!!!!!!!!!!!!!!!!!\n"
        return (PACKET_DEAD_PERIOD < (self.brain.time - self.lastPacketTime))

    def __str__(self):
        return "I am player number " + self.playerNumber

    def __eq__(self, other):
        return self.playerNumber == other.playerNumber

    def __ne__(self, other):
        return self.playerNumber != other.playerNumber
