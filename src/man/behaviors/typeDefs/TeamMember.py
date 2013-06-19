from objects import (RobotLocation, Location, RelRobotLocation)
from math import fabs, degrees
import noggin_constants as NogginConstants

OPP_GOAL = Location(NogginConstants.OPP_GOALBOX_RIGHT_X,
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
DEBUG_DETERMINE_DEST_TIME = False
SEC_TO_MILLIS = 1000.0
WALK_SPEED = 20.00 #cm/sec          # How fast we walk
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
        self.chaseTime = 0
        self.defenderTime = 0
        self.offenderTime = 0
        self.middieTime = 0
        self.inKickingState = False

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
        self.chaseTime = info.chase_time
        self.defenderTime = info.defender_time
        self.offenderTime = info.offender_time
        self.middieTime = info.middie_time
        self.inKickingState = info.in_kicking_state
        self.active = info.active

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
        self.role = self.brain.play.role
        if ( not self.brain.player.gameState == 'gameReady'):
            self.chaseTime = self.determineChaseTime()
        self.inKickingState = self.brain.player.inKickingState

        self.active = (not self.isPenalized() and
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
        return self.getRelativeBearing(OPP_GOAL)

    def determineChaseTime(self):
        """
        Attempt to define a time in seconds to get to the ball.
        Can give penalties in certain situations.
        @ return: returns a time in seconds with penalties.
        Note: Don't give bonuses. It can result in negative chase times
              which can screw up the math later on. --Wils (06/25/11)
        """
        relLocToBall = RelRobotLocation(self.brain.ball.rel_x,
                                        self.brain.ball.rel_y,
                                        self.brain.ball.bearing_deg)

        time = self.determineTimeToDest(relLocToBall)

        # Give a penalty for not seeing the ball if we aren't in a kickingState
        if (self.brain.ball.vis.frames_off > 60 and # TODO: unify this constant with shouldFindBall
            not self.brain.player.inKickingState):
            time += BALL_OFF_PENALTY

        if DEBUG_DETERMINE_DEST_TIME:
            print "\tChase time after ball on bonus " + str(time)

        return time

    def determineTimeToDest(self, dest):
        """
        Returns the approxiamte time in seconds to reach the given destination.
        @param dest: a RelRobotLocation.
        """

        time = (dest.dist / WALK_SPEED)

        if DEBUG_DETERMINE_DEST_TIME:
            print "\tDest time base is " + str(time)

        # Add a penalty for being fallen over
        time += 20 * (self.brain.player.currentState == 'fallen')

        if DEBUG_DETERMINE_DEST_TIME:
            print "\tDest time after fallen over penalty " + str(time)

        # Add a penalty for not facing the destination
        time += fabs(dest.bearing / 60) # 3 seconds to spin 180

        # Add a penalty for turning once at the destination
        time += fabs((dest.bearing - dest.relH) / 60) # 3 seconds to spin 180

        if DEBUG_DETERMINE_DEST_TIME:
            print "\tDest time after bearing penalty " + str(time)

        return time

    def isTeammateRole(self, roleToTest):
        return (self.role == roleToTest)

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
