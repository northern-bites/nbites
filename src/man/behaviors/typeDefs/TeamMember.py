from objects import (RobotLocation, Location, RelRobotLocation)
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
        RobotLocation.__init__(self, 0.0, 0.0, 0.0)
        self.playerNumber = 0
        self.ballDist = 0
        self.ballBearing = 0
        self.ballOn = False
        self.role = 1
        self.inKickingState = False
        self.claimedBall = False

        #other info we want stored
        self.brain = tbrain # brain instance
        self.active = True

    # @param info: an instance of a WorldModel protobuf
    def update(self, info):
        '''
        Updates information from latest Comm
        '''
        self.x = info.my_x
        self.y = info.my_y
        self.h = info.my_h
        self.ballOn = info.ball_on
        self.ballDist = info.ball_dist
        self.ballBearing = info.ball_bearing
        self.role = info.role
        self.inKickingState = info.in_kicking_state
        self.active = info.active
        self.claimedBall = info.claimed_ball
        if self.claimedBall:
            self.claimTime = time.time()

        # calculates ball localization distance, bearing
        self.bearingToGoal = self.getBearingToGoal()

    def updateMe(self):
        """
        updates my information as a teammate (since we may not get our own packets)
        """
        ball = self.brain.ball

        self.x = self.brain.loc.x
        self.y = self.brain.loc.y
        self.h = self.brain.loc.h

        self.ballOn = ball.vis.frames_on > 0
        self.ballDist = ball.distance
        self.ballBearing = ball.bearing_deg
        self.role = self.brain.player.role
        self.inKickingState = self.brain.player.inKickingState
        self.claimedBall = self.brain.player.claimedBall

        self.active = (not self.isPenalized() and
                       not self.brain.player.currentState == 'afterPenalty' and
                       not (self.brain.playerNumber == 1 and
                            self.brain.player.returningFromPenalty))

    def reset(self):
        '''Reset all important Teammate variables'''
        self.x = 0
        self.y = 0
        self.h = 0
        self.ballOn = 0
        self.ballDist = 0
        self.ballBearing = 0
        self.role = None
        self.active = False

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

    def isDefaultChaser(self):
        return (self.playerNumber == DEFAULT_CHASER_NUMBER)

    def isDefaultOffender(self):
        return (self.playerNumber == DEFAULT_OFFENDER_NUMBER)

    def isDefaultDefender(self):
        return (self.playerNumber == DEFAULT_DEFENDER_NUMBER)

    def isDefaultMiddie(self):
        return (self.playerNumber == DEFAULT_MIDDIE_NUMBER)

    def __str__(self):
        return "I am player number " + self.playerNumber

    def __eq__(self, other):
        return self.playerNumber == other.playerNumber

    def __ne__(self, other):
        return self.playerNumber != other.playerNumber
