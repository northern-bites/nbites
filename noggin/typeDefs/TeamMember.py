
from math import (hypot, degrees)

from .Location import RobotLocation
from .. import NogginConstants
from ..util import MyMath
import time

PACKET_DEAD_PERIOD = 5 # TO-DO: look at shortening so it replaces penalized
DEFAULT_GOALIE_NUMBER = 1
DEFAULT_DEFENDER_NUMBER = 2
DEFAULT_CHASER_NUMBER = 3


class TeamMember(RobotLocation):
    """class for keeping track of teammates' info """

    def __init__(self,tbrain=None):
        '''variables include lots from the Packet class'''
        RobotLocation.__init__()
        # things in the Packet()
        self.playerNumber = 0
        self.h = 0
        self.uncertX = 0
        self.uncertY = 0
        self.uncertH = 0
        self.ballX = 0
        self.ballY = 0
        self.ballUncertX = 0
        self.ballUncertY = 0
        self.ballDist = 0
        self.role = None
        self.subRole = None
        self.chaseTime = 0
        self.lastPacketTime = time.time()

        #other info we want stored
        self.ballLocDist = 0
        self.ballLocBearing = 0
        self.brain = tbrain # brain instance
        self.active = True
        self.grabbing = False
        self.dribbling = False

    def update(self,packet):
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
        self.uncertX = packet.uncertX
        self.uncertY = packet.uncertY
        self.uncertH = packet.uncertH
        self.ballX = packet.ballX
        self.ballY = packet.ballY
        self.ballUncertX = packet.ballUncertX
        self.ballUncertY = packet.ballUncertY
        self.ballDist = packet.ballDist
        self.role = packet.role
        self.subRole = packet.subRole
        self.chaseTime = packet.chaseTime

        # calculates ball localization distance, bearing
        self.ballLocDist = self.getDistToBall()
        self.ballLocBearing = self.getBearingToBall()
        self.bearingToGoal = self.getBearingToGoal()
        self.active = True
        self.grabbing = (0 < self.ballDist <=
                        NogginConstants.BALL_TEAMMATE_DIST_GRABBING) or\
                        (0 < self.ballLocDist <=
                        NogginConstants.BALL_TEAMMATE_DIST_GRABBING)
        #potential problem when goalie is grabbing?
        #only going to be dribbling or grabbing if you see the ball
        self.dribbling = (0 < self.ballDist <=
                          NogginConstants.BALL_TEAMMATE_DIST_DRIBBLING) or\
                          (0 < self.ballLocDist <=
                          NogginConstants.BALL_TEAMMATE_DIST_DRIBBLING)
        self.lastPacketTime = self.brain.playbook.pb.time


    def updateMe(self):
        '''
        updates my information as a teammate (since we don't get our own
        packets)
        '''
        my = self.brain.my
        ball = self.brain.ball
        pb = self.brain.playbook

        #self.playerNumber = self.brain.my.playerNumber
        self.x = my.x
        self.y = my.y
        self.h = my.h
        self.uncertX = my.uncertX
        self.uncertY = my.uncertY
        self.uncertH = my.uncertH
        self.ballX = ball.x
        self.ballY = ball.y
        self.ballUncertX = ball.uncertX
        self.ballUncertY = ball.uncertY
        self.ballDist = ball.dist
        self.role = self.brain.play.role
        self.subRole = self.brain.play.subRole
        self.chaseTime = pb.pb.determineChaseTime()

        self.ballLocDist = ball.locDist
        self.ballLocBearing = ball.locBearing
        self.active = (not self.brain.gameController.currentState ==
                       'gamePenalized')
        #only going to be dribbling or grabbing if you see the ball
        self.dribbling = (0 < self.ballDist <=
                          NogginConstants.BALL_TEAMMATE_DIST_DRIBBLING)
        self.grabbing = (0 < self.ballDist <=
                          NogginConstants.BALL_TEAMMATE_DIST_GRABBING)
        self.lastPacketTime = pb.pb.time

    def reset(self):
        '''Reset all important Teammate variables'''
        self.x = 0
        self.y = 0
        self.h = 0
        self.uncertX = 0
        self.uncertY = 0
        self.uncertH = 0
        self.ballX = 0
        self.ballY = 0
        self.ballUncertX = 0
        self.ballUncertY = 0
        self.ballDist = 0
        self.role = None
        self.subRole = None
        self.ballLocDist = 0
        self.ballLocBearing = 0
        self.active = False
        self.grabbing = False
        self.dribbling = False

    def getBearingToGoal(self):
        '''returns bearing to goal'''
        return self.getOthersRelativeBearing(self.x, self.y, self.h,
                                           NogginConstants.OPP_GOALBOX_LEFT_X,
                                           NogginConstants.OPP_GOALBOX_MIDDLE_Y)

    def getDistToBall(self):
        '''
        returns teammate distance to ball in centimeters.
        -based on its own localization but my own ball estimates
        '''
        return hypot(self.brain.ball.x - self.x,
                     self.brain.ball.y - self.y)

    def getBearingToBall(self):
        '''
        returns teammate bearing to the ball in degrees.
        -based on its own localization but my own ball estimates
        -return values is between -180,180
        '''
        return self.getOthersRelativeBearing(self.x, self.y, self.h,
                                             self.brain.ball.x,
                                             self.brain.ball.y)

    def getOthersRelativeBearing(self,playerX,playerY,playerH,x,y):
        """
        get another player's bearing to a point (x,y)
        """
        return MyMath.sub180Angle(playerH -(degrees(MyMath.safe_atan2(
                        y - playerY, x - playerX)) - 90.0))

    def hasBall(self):
        return (self.dribbling or self.grabbing)

    def isTeammateRole(self, roleToTest):
        return (self.role == roleToTest)

    def isDefaultGoalie(self):
        return (self.playerNumber == DEFAULT_GOALIE_NUMBER)

    def isDefaultChaser(self):
        return (self.playerNumber == DEFAULT_CHASER_NUMBER)

    def isDefaultDefender(self):
        return (self.playerNumber == DEFAULT_DEFENDER_NUMBER)

    def isPenalized(self):
        '''
        this checks GameController to see if a player is penalized.
        this check is more redundant than anything, because our players stop
        sending packets when they are penalized, so they will most likely
        fall under the isTeammateDead() check anyways.
        '''
        #penalty state is the first item the player tuple [0]
        #penalty state == 1 is penalized
        return (
            self.brain.gameController.gc.players(self.playerNumber)[0]!=0
           )

    def isDead(self):
        '''
        returns True if teammates' last timestamp is sufficiently behind ours.
        however, the dog could still be on but sending really laggy packets.
        '''
        return (PACKET_DEAD_PERIOD <
                (self.brain.playbook.pb.time - self.lastPacketTime))

    def __str__(self):
        return "I am player number " + self.playerNumber
