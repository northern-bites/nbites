from objects import (RobotLocation, Location, RelLocation, RelRobotLocation)
from math import fabs, degrees
import time
import noggin_constants as NogginConstants

DEFAULT_GOALIE_NUMBER = 1
DEFAULT_DEFENDER_NUMBER = 2
DEFAULT_MIDDIE_NUMBER = 3
DEFAULT_OFFENDER_NUMBER = 4
DEFAULT_CHASER_NUMBER = 5

class TeamMember(RobotLocation):
    """class for keeping track of teammates' info """

    def __init__(self, tbrain=None):
        self.playerNumber = 0

        RobotLocation.__init__(self, 0.0, 0.0, 0.0)
        self.locUncert = 0
        # TODO use location objects
        self.walkingToX = 0
        self.walkingToY = 0
        self.ballOn = False
        self.ballAge = 0
        self.ballDist = 0
        self.ballBearing = 0
        self.ballVelX = 0
        self.ballVelY = 0
        self.ballUncert = 0
        self.role = 0
        self.inKickingState = False
        self.kickingToX = 0
        self.kickingToY = 0
        self.fallen = False
        self.active = True
        self.claimedBall = False

        self.frameSinceActive = 0
        self.lastTimestamp = 0
        self.framesWithoutPacket = -1

        self.brain = tbrain # brain instance

    # @param info: an instance of a WorldModel protobuf
    def update(self, info):
        '''
        Updates information from latest Comm
        '''
        self.x = info.my_x
        self.y = info.my_y
        self.h = info.my_h
        self.locUncert = info.my_uncert
        self.walkingToX = info.walking_to_x
        self.walkingToY = info.walking_to_y
        self.ballOn = info.ball_on
        self.ballAge = info.ball_age
        self.ballDist = info.ball_dist
        self.ballBearing = info.ball_bearing
        self.ballVelX = info.ball_vel_x
        self.ballVelY = info.ball_vel_y
        self.ballUncert = info.ball_uncert
        self.role = info.role
        self.inKickingState = info.in_kicking_state
        self.kickingToX = info.kicking_to_x
        self.kickingToY = info.kicking_to_y
        self.active = info.active
        self.fallen = info.fallen
        self.claimedBall = info.claimed_ball

        if self.active:
            self.frameSinceActive = 0
        else:
            self.frameSinceActive += 1

        if info.timestamp != self.lastTimestamp:
            self.lastTimestamp = info.timestamp
            self.framesWithoutPacket = 0
        elif self.framesWithoutPacket != -1:
            self.framesWithoutPacket += 1

        # Calculated from protobuf
        if self.claimedBall:
            self.claimTime = time.time()
        self.bearingToGoal = self.getBearingToGoal()

    def updateMe(self):
        """
        updates my information as a teammate (since we may not get our own packets)
        """
        ball = self.brain.ball

        self.x = self.brain.loc.x
        self.y = self.brain.loc.y
        self.h = self.brain.loc.h

        self.locUncert = self.brain.locUncert

        if self.brain.nav.destination:
            if (isinstance(self.brain.nav.destination,RelLocation) or 
                isinstance(self.brain.nav.destination,RelRobotLocation)):
                self.walkingToX = self.brain.loc.x + self.brain.nav.destination.relX
                self.walkingToY = self.brain.loc.y + self.brain.nav.destination.relY
            else:
                self.walkingToX = self.brain.nav.destination.x
                self.walkingToY = self.brain.nav.destination.y
        else:
            self.walkingToX = self.brain.loc.x
            self.walkingToY = self.brain.loc.y

        self.ballOn = ball.vis.frames_on > 0
        # TODO -1 when ball has not been seen
        self.ballAge = ball.vis.frames_off*30
        self.ballDist = ball.distance
        self.ballBearing = ball.bearing_deg

        self.ballVelX = ball.vel_x
        self.ballVelY = ball.vel_y

        # TODO use covariance matrices
        self.ballUncert = 0

        self.role = self.brain.player.role

        self.inKickingState = self.brain.player.inKickingState
        if self.inKickingState:
            self.kickingToX = self.brain.player.kick.destinationX
            self.kickingToY = self.brain.player.kick.destinationY

        self.active = (not self.isPenalized() and
                       not self.brain.player.currentState == 'afterPenalty' and
                       not (self.brain.playerNumber == 1 and
                            self.brain.player.returningFromPenalty))
        self.fallen = self.brain.fallController.falling or self.brain.fallController.fell

        self.claimedBall = self.brain.player.claimedBall

    def reset(self):
        '''Reset all important Teammate variables'''
        self.x = 0
        self.y = 0
        self.h = 0
        self.locUncert = 0
        self.walkingToX = 0
        self.walkingToY = 0
        self.ballOn = False
        self.ballAge = 0
        self.ballDist = 0
        self.ballBearing = 0
        self.ballVelX = 0
        self.ballVelY = 0
        self.ballUncert = 0
        self.role = None
        self.inKickingState = False
        if self.inKickingState:
            self.kickingToX = 0
            self.kickingToY = 0
        self.active = False
        self.fallen = False
        self.claimedBall = False
        self.frameSinceActive = 0
        self.lastTimestamp = 0
        self.framesWithoutPacket = 0

    def getBearingToGoal(self):
        """returns bearing to goal"""
        opp_goal = Location(NogginConstants.OPP_GOALBOX_RIGHT_X,
                            NogginConstants.OPP_GOALBOX_MIDDLE_Y)
        return self.getRelativeBearing(opp_goal)

    def isTeammateRole(self, roleToTest):
        return (self.role == roleToTest)

    def isPenalized(self):
        """
        this checks GameController to see if a player is penalized.
        """
        return (self.brain.gameController.penalized)

    def isDefaultGoalie(self):
        return (self.playerNumber == DEFAULT_GOALIE_NUMBER)

    def __str__(self):
        return "I am player number " + str(self.playerNumber)

    def __eq__(self, other):
        return self.playerNumber == other.playerNumber

    def __ne__(self, other):
        return self.playerNumber != other.playerNumber
