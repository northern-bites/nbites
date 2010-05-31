
from math import (hypot, degrees)

from .Location import (RobotLocation, Location)
from .. import NogginConstants
from ..util import MyMath
import time

OPP_GOAL = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                    NogginConstants.OPP_GOALBOX_MIDDLE_Y)
PACKET_DEAD_PERIOD = 2
DEFAULT_GOALIE_NUMBER = 1
DEFAULT_DEFENDER_NUMBER = 2
DEFAULT_CHASER_NUMBER = 3
DEBUG_DETERMINE_CHASE_TIME = True
BALL_ON_BONUS = 1000.
CHASE_SPEED = 7.00
SEC_TO_MILLIS = 1000.0
# penalty is: (ball_dist*heading)/scale
PLAYER_HEADING_PENALTY_SCALE = 300.0 # max 60% of distance
# penalty is: (ball_dist*ball_bearing)/scale
BALL_BEARING_PENALTY_SCALE = 200.0 # max 90% of distance
NO_VISUAL_BALL_PENALTY = 2 # centimeter penalty for not seeing the ball


class TeamMember(RobotLocation):
    """class for keeping track of teammates' info """

    def __init__(self, tbrain=None):
        '''variables include lots from the Packet class'''
        RobotLocation.__init__(self)
        # things in the Packet()
        self.playerNumber = 0
        self.ballDist = 0
        self.ballBearing = 0
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
        self.role = packet.role
        self.subRole = packet.subRole
        self.chaseTime = packet.chaseTime

        # calculates ball localization distance, bearing
        self.bearingToGoal = self.getBearingToGoal()
        self.active = True
        self.grabbing = self.ballDist <= \
                        NogginConstants.BALL_TEAMMATE_DIST_GRABBING

        #potential problem when goalie is grabbing?
        #only going to be dribbling or grabbing if you see the ball
        self.dribbling = self.ballDist <= \
                          NogginConstants.BALL_TEAMMATE_DIST_DRIBBLING

        self.lastPacketTime = time.time()#self.brain.playbook.pb.time


    def updateMe(self):
        """
        updates my information as a teammate (since we don't get our own
        packets)
        """

        my = self.brain.my
        ball = self.brain.ball
        pb = self.brain.playbook

        #self.playerNumber = self.brain.my.playerNumber
        self.x = my.x
        self.y = my.y
        self.h = my.h
        self.ballDist = ball.dist
        self.ballBearing = 0
        self.role = self.brain.play.role
        self.subRole = self.brain.play.subRole
        self.chaseTime = self.determineChaseTime()

        self.active = (not self.brain.gameController.currentState ==
                       'gamePenalized')

        self.dribbling = self.ballDist <= \
                         NogginConstants.BALL_TEAMMATE_DIST_DRIBBLING
        self.grabbing = self.ballDist <= \
                        NogginConstants.BALL_TEAMMATE_DIST_GRABBING
        self.lastPacketTime = self.brain.time

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
        Metric for deciding chaser.
        Attempt to define a time to get to the ball.
        Can give bonuses or penalties in certain situations.
        """
        time = 0.0

        ## if DEBUG_DETERMINE_CHASE_TIME:
        ##     self.printf("DETERMINE CHASE TIME DEBUG")

        time += (self.ballDist / CHASE_SPEED) *\
            SEC_TO_MILLIS

        if DEBUG_DETERMINE_CHASE_TIME:
            self.brain.out.printf("\tChase time base is " + str(time))

        # Give a bonus for seeing the ball
        if self.brain.ball.on:
            time -= BALL_ON_BONUS

        if DEBUG_DETERMINE_CHASE_TIME:
            self.brain.out.printf("\tChase time after ball on bonus " + str(time))

        # Add a penalty for being fallen over
        time += (self.brain.fallController.getTimeRemainingEst() *
                 SEC_TO_MILLIS)

        if DEBUG_DETERMINE_CHASE_TIME:
            self.brain.out.printf("\tChase time after fallen over penalty " + str(time))
            self.brain.out.printf("")

        return time

    def hasBall(self):
        return self.grabbing

    def isTeammateRole(self, roleToTest):
        return (self.role == roleToTest)

    def isDefaultGoalie(self):
        return (self.playerNumber == DEFAULT_GOALIE_NUMBER)

    def isDefaultChaser(self):
        return (self.playerNumber == DEFAULT_CHASER_NUMBER)

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
        however, the dog could still be on but sending really laggy packets.
        """
        return (PACKET_DEAD_PERIOD <
                (self.brain.time - self.lastPacketTime))

        return (PACKET_DEAD_PERIOD <
                (self.brain.playbook.pb.time - self.lastPacketTime))

    def __str__(self):
        return "I am player number " + self.playerNumber
