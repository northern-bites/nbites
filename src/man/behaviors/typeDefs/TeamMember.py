from objects import (RobotLocation, Location)
from math import fabs, degrees
import noggin_constants as NogginConstants

OPP_GOAL = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                    NogginConstants.OPP_GOALBOX_MIDDLE_Y)
OPP_GOAL_LEFT_POST = Location(NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                              NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y)
OPP_GOAL_RIGHT_POST = Location(NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_X,
                              NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y)

DEFAULT_GOALIE_NUMBER = 1
DEFAULT_DEFENDER_NUMBER = 2
DEFAULT_MIDDIE_NUMBER = 3
DEFAULT_OFFENDER_NUMBER = 4
DEFAULT_CHASER_NUMBER = 5
DEBUG_DETERMINE_CHASE_TIME = False
SEC_TO_MILLIS = 1000.0
CHASE_SPEED = 20.00 #cm/sec          # How fast we walk
CHASE_TIME_SCALE = 0.45              # How much new measurement is used in IIR.
BALL_OFF_PENALTY = 100.              # Big penalty for not seeing the ball.

# Behavior constants
BALL_TEAMMATE_DIST_GRABBING = 35
BALL_TEAMMATE_BEARING_GRABBING = 45.
BALL_TEAMMATE_DIST_DRIBBLING = 20

# Ball on?
BALL_FRAMES = 20

class TeamMember(RobotLocation):
    """class for keeping track of teammates' info """

    def __init__(self, tbrain=None):
        RobotLocation.__init__(self, 0.0, 0.0, 0.0)
        self.playerNumber = 0
        self.ballDist = 0
        self.ballBearing = 0
        self.ballOn = False
        self.role = None
        self.subRole = None
        self.chaseTime = 0

        #other info we want stored
        self.brain = tbrain # brain instance
        self.active = True
        self.grabbing = False
        self.dribbling = False

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
        self.subRole = info.sub_role
        self.chaseTime = info.chase_time
        self.active = info.active

        # calculates ball localization distance, bearing
        self.bearingToGoal = self.getBearingToGoal()
        self.grabbing = (self.active and self.ballDist <=
                         BALL_TEAMMATE_DIST_GRABBING) and \
                         (fabs(self.ballBearing) <
                          BALL_TEAMMATE_BEARING_GRABBING)

        #potential problem when goalie is grabbing?
        #only going to be dribbling or grabbing if you see the ball
        self.dribbling = (self.active and self.ballDist <=
                          BALL_TEAMMATE_DIST_DRIBBLING)

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
        self.role = self.brain.play.role
        self.subRole = self.brain.play.subRole
        self.chaseTime = self.determineChaseTime()

        self.active = (not self.isPenalized())

        self.dribbling = (self.active and self.ballDist <=
                         BALL_TEAMMATE_DIST_DRIBBLING)

        self.grabbing = (self.active and self.ballDist <=
                         BALL_TEAMMATE_DIST_GRABBING) and \
                         (fabs(self.ballBearing) <
                          BALL_TEAMMATE_BEARING_GRABBING)

    def reset(self):
        '''Reset all important Teammate variables'''
        self.x = 0
        self.y = 0
        self.h = 0
        self.ballOn = 0
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
        @ return: returns a time in seconds with penalties.
        Note: Don't give bonuses. It can result in negative chase times
              which can screw up the math later on. --Wils (06/25/11)
        """
        t = (self.ballDist / CHASE_SPEED)

        if DEBUG_DETERMINE_CHASE_TIME:
            print "\tChase time base is " + str(t)

        # Give a penalty for not seeing the ball if we aren't in a kickingState
        if (self.brain.ball.vis.frames_off > 3 and
            not self.brain.player.inKickingState):
            t += BALL_OFF_PENALTY

        if DEBUG_DETERMINE_CHASE_TIME:
            print "\tChase time after ball on bonus " + str(t)

        # Add a penalty for being fallen over
        t += 20 * (self.brain.player.currentState == 'fallen')

        if DEBUG_DETERMINE_CHASE_TIME:
            print "\tChase time after fallen over penalty " + str(t)

        # Add a penalty for not facing the ball
        t += fabs(self.brain.ball.bearing_deg) / 60  # 3 seconds to spin 180

        if DEBUG_DETERMINE_CHASE_TIME:
            print "\tChase time after ball-bearing penalty "+str(t)

        # Filter by IIR to reduce noise
        # Actually this is probably terrible.
        #t = t * CHASE_TIME_SCALE + (1.0 -CHASE_TIME_SCALE) * self.chaseTime

        if DEBUG_DETERMINE_CHASE_TIME:
            print "\tChase time after filter " +str(t)
            print ""

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

    def isDefaultMiddie(self):
        return (self.playerNumber == DEFAULT_MIDDIE_NUMBER)

    def isPenalized(self):
        """
        this checks GameController to see if a player is penalized.
        """
        return (self.brain.interface.gameState.team(self.brain.gameController.teamColor).player(self.playerNumber-1).penalty
                or self.brain.player.currentState == 'afterPenalty')

    def __str__(self):
        return "I am player number " + self.playerNumber

    def __eq__(self, other):
        return self.playerNumber == other.playerNumber

    def __ne__(self, other):
        return self.playerNumber != other.playerNumber
