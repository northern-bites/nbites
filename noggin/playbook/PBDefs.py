
from math import (hypot, degrees)

from .. import NogginConstants
from . import PBConstants
from ..util import MyMath
import time

class Teammate:
    '''class for keeping track of teammates' info'''
    def __init__(self,tbrain=None):
        '''variables include lots from the Packet class'''

        # things in the Packet()
        self.playerNumber = 0
        self.timeStamp = 0
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
        self.ballLocDist = 0
        self.ballLocBearing = 0
        self.calledRole = 0
        self.lastPacketTime = 0

        self.brain = tbrain        # brain instance
        self.role = 0 # known role
        self.inactive = False # dead basically just means inactive
        self.grabbing = False # dog is grabbing
        self.dribbling = False # dog is dribbling
        self.kicking = False # dog is kicking[
        self.chaseTime = 0 # estimated time to chase the ball
        self.bearingToGoal = 0 # bearing to goal

    def update(self,packet):
        '''
        receives a packet, updates teammate information. packets received
        have already been verified by timestamp system, so we can assume
        they are LEGIT.
        '''
        
        # stores packet information locally
        self.timeStamp = time.time()
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
        self.calledRole = packet.role
        self.calledSubRole = packet.calledSubRole
        if self.isChaser():
            self.calledRole = PBConstants.CHASER
        elif self.isDefender():
            self.calledRole = PBConstants.DEFENDER
        self.chaseTime = packet.chaseTime

        # calculates ball localization distance, bearing
        self.ballLocDist = self.getDistToBall()
        self.ballLocBearing = self.getBearingToBall()
        self.bearingToGoal = self.getBearingToGoal()

        self.inactive = False

        self.grabbing = (packet.ballDist <= 
                         NogginConstants.BALL_TEAMMATE_DIST_GRABBING)
        self.dribbling = (packet.ballDist <= 
                          NogginConstants.BALL_TEAMMATE_DIST_DRIBBLING)
        self.kicking = False
        #(packet.ballDist == 
        #                NogginConstants.BALL_TEAMMATE_DIST_KICKING)

        self.lastPacketTime = time.time()

    def updateMe(self):
        '''updates my information as a teammate (since we don't get our own 
        packets)'''
        self.playerNumber = self.brain.my.playerNumber
        self.timeStamp = time.time()
        self.x = self.brain.my.x
        self.y = self.brain.my.y
        self.h = self.brain.my.h
        self.uncertX = self.brain.my.uncertX
        self.uncertY = self.brain.my.uncertY
        self.uncertH = self.brain.my.uncertH
        self.ballX = self.brain.ball.x
        self.ballY = self.brain.ball.y
        self.ballUncertX = self.brain.ball.uncertX
        self.ballUncertY = self.brain.ball.uncertY
        self.ballDist = self.brain.ball.dist
        self.ballLocDist = self.brain.ball.locDist
        self.ballLocBearing = self.brain.ball.locBearing
        self.inactive = False
        self.grabbing = (self.ballDist == 
                         NogginConstants.BALL_TEAMMATE_DIST_GRABBING)
        self.dribbling = (self.ballDist == 
                          NogginConstants.BALL_TEAMMATE_DIST_DRIBBLING)
        #self.kicking = (self.ballDist == 
        #                NogginConstants.BALL_TEAMMATE_DIST_KICKING)

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
        '''get another player's bearing to a point (x,y)'''
        return MyMath.sub180Angle(playerH - (degrees(MyMath.safe_atan2(y - playerY,
            x - playerX)) - 90.0))
           
    def reset(self):
        '''Reset all important Teammate variables'''
        #self.playerNumber = 0 # doesn't reset player number
        self.timeStamp = 0
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
        self.ballLocDist = 0
        self.ballLocBearing = 0
        self.grabbing = False
        self.kicking = False
        self.dribbling = False
        self.role = 0 # known role
        self.inactive = True # dead basically just means inactive
    
    def isChaser(self):
        return (self.calledSubRole == PBConstants.CHASE_NORMAL or 
            self.calledSubRole == PBConstants.CHASE_AROUND_BOX)

    def isDefender(self):
        return (self.calledSubRole == PBConstants.STOPPER or
                self.calledSubRole == PBConstants.DEEP_STOPPER or
                self.calledSubRole == PBConstants.SWEEPER or
                self.calledSubRole == PBConstants.LEFT_DEEP_BACK or
                self.calledSubRole == PBConstants.RIGHT_DEEP_BACK)
    
    def isPenalized(self):
        '''
        this checks GameController to see if a player is penalized.
        this check is more redundant than anything, because our players stop
        sending packets when they are penalized, so they will most likely
        fall under the isTeammateDead() check anyways.
        '''
        return (self.brain.gameController.currentState =='gamePenalized')
        #penalty state is the first item the player tuple [0]
        #penalty state == 1 is penalized
        #return (
        #    self.brain.gameController.gc.players(
        #        self.playerNumber)[0]!=0
        #   )

    def isDead(self):
        '''
        returns True if teammates' last timestamp is sufficiently behind ours.
        however, the dog could still be on but sending really laggy packets.
        '''
        return (PBConstants.PACKET_DEAD_PERIOD <
                    time.time() - self.lastPacketTime)

    def hasBall(self):
        '''
        returns true if we have the ball
        '''
        return (self.grabbing or self.dribbling or self.kicking)
