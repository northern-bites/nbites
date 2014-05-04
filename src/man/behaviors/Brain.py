import time
import sys
import math

# Redirect standard error to standard out
_stderr = sys.stderr
sys.stderr = sys.stdout

# Packages and modules from super-directories
import noggin_constants as Constants
from objects import Location, RobotLocation

# Modules from this directory
from . import Leds
from . import robots
from . import GameController
from . import FallController

# Packages and modules from sub-directories
from .headTracker import HeadTracker
from .typeDefs import TeamMember
from .navigator import Navigator
from .players import Switch
from .kickDecider import KickDecider

# Import message protocol buffers and interface
import interface
import LedCommand_proto
import GameState_proto
import WorldModel_proto
import RobotLocation_proto
import BallModel_proto
import PMotion_proto
import MotionStatus_proto
import VisionRobot_proto
import VisionField_proto
import ButtonState_proto
import FallStatus_proto
import StiffnessControl_proto

class Brain(object):
    """
    Class brings all of our components together and runs the behaviors
    """

    def __init__(self, teamNum, playerNum):
        """
        Class constructor
        """

        # Parse arguments
        self.playerNumber = playerNum
        self.teamNumber = teamNum

        self.counter = 0
        self.time = time.time()

        # Initalize the leds and game controller
        self.leds = Leds.Leds(self)

        # Initialize fallController
        self.fallController = FallController.FallController(self)

        # Retrieve our robot identification and set per-robot parameters
        self.CoA = robots.get_certificate()

        # coa is Certificate of Authenticity (to keep things short)
        print '\033[32m'+str(self.CoA)                              +'\033[0m'
        print '\033[32m'+"GC:  I am on team "+str(self.teamNumber)  +'\033[0m'
        print '\033[32m'+"GC:  I am player  "+str(self.playerNumber)+'\033[0m'

        # Information about the environment
        self.ball = None
        self.initTeamMembers()
        self.motion = None
        self.game = None
        self.locUncert = 0

        # FSAs
        self.player = Switch.selectedPlayer.SoccerPlayer(self)
        self.tracker = HeadTracker.HeadTracker(self)
        self.nav = Navigator.Navigator(self)

        # Not FSAs
        self.gameController = GameController.GameController(self)
        self.kickDecider = KickDecider.KickDecider(self)

        self.player.claimedBall = False

        # Message interface
        self.interface = interface.interface

        # HACK for dangerous ball flipping loc
        self.noFlipFilter = []
        self.flipFilter = []

    def initTeamMembers(self):
        self.teamMembers = []
        for i in xrange(Constants.NUM_PLAYERS_PER_TEAM):
            mate = TeamMember.TeamMember(self)
            mate.playerNumber = i + 1
            self.teamMembers.append(mate)

##
##--------------CONTROL METHODS---------------##
##
    def profile(self):
        if self.counter == 0:
            cProfile.runctx('self.run()',  self.__dict__, locals(),
                            'pythonStats')
            self.p = pstats.Stats('pythonStats')

        elif self.counter < 3000:
            self.p.add('pythonStats')
            cProfile.runctx('self.run()',  self.__dict__, locals(),
                            'pythonStats')

        elif self.counter == 3000:
            self.p.strip_dirs()
            self.p.sort_stats('cumulative')
            ## print 'PYTHON STATS:'
            ## self.p.print_stats()
            ## print 'OUTGOING CALLEES:'
            ## self.p.print_callees()
            ## print 'OUTGOING CALLEES:'
            ## self.p.print_callers()
            self.p.dump_stats('pythonStats')

        self.counter += 1

    def run(self):
        """
        Main control loop
        """
        # Update Environment
        self.time = time.time()

        # Update objects
        self.updateVisionObjects()
        self.updateMotion()
        self.updateLoc()
        self.getCommUpdate()

        # Behavior stuff
        # Order here is very important
        self.gameController.run()
        self.fallController.run()
        self.player.run()
        self.tracker.run()
        self.nav.run()

        # HACK for dangerous ball flipping loc
        self.flipLocFilter()

        # Set LED message
        self.leds.processLeds()

        # Set myWorldModel for Comm
        self.updateComm()

        # Flush the output
        sys.stdout.flush()

    def updateComm(self):
        output = self.interface.myWorldModel

        output.timestamp = int(self.time * 1000)

        output.my_x = self.loc.x
        output.my_y = self.loc.y
        output.my_h = self.loc.h

        output.my_uncert = self.locUncert

        output.ball_on = self.ball.vis.on

        output.ball_dist = self.ball.distance
        output.ball_bearing = self.ball.bearing_deg

        #TODO get actual uncertainties, or transition to rel_x
        output.ball_dist_uncert = 0
        output.ball_bearing_uncert = 0

        output.role = self.teamMembers[self.playerNumber-1].role

        output.active = self.teamMembers[self.playerNumber-1].active
        output.in_kicking_state = self.player.inKickingState

        output.claimed_ball = self.player.claimedBall

    def getCommUpdate(self):
        self.teamMembers[self.playerNumber - 1].updateMe()
        self.game = self.interface.gameState
        for i in range(len(self.teamMembers)):
            if (i == self.playerNumber - 1):
                continue
            self.teamMembers[i].update(self.interface.worldModelList()[i])

    def updateMotion(self):
        self.motion = self.interface.motionStatus

    def updateVisionObjects(self):
        """
        Update estimates of robot and ball positions on the field
        """
        self.ball = self.interface.filteredBall
        if (self.player.gameState == 'gameReady'
            or self.player.gameState == 'gameSet'):
            self.ball.x = Constants.CENTER_FIELD_X
            self.ball.y = Constants.CENTER_FIELD_Y

        self.yglp = self.interface.visionField.goal_post_l.visual_detection
        self.ygrp = self.interface.visionField.goal_post_r.visual_detection

    def activeTeamMates(self):
        activeMates = 0
        for i in xrange(Constants.NUM_PLAYERS_PER_TEAM):
            mate = self.teamMembers[i]
            if mate.active:
                activeMates += 1
        return activeMates

    def updateLoc(self):
        """
        Make Loc info a RobotLocation.
        """
        self.loc = RobotLocation(self.interface.loc.x,
                                 self.interface.loc.y,
                                 self.interface.loc.h * (180. / math.pi))
        self.locUncert = self.interface.loc.uncert

    def resetLocTo(self, x, y, h):
        """
        Sends a reset request to loc to reset to given x, y, h
        """
        self.interface.resetLocRequest.x = x
        self.interface.resetLocRequest.y = y
        self.interface.resetLocRequest.h = h * (math.pi / 180.)
        self.interface.resetLocRequest.timestamp = int(self.time * 1000)

    def resetInitialLocalization(self):
        """
        Reset loc according to team number and team color.
        Assumes manual positioning.
        Note: Loc uses truly global coordinates, and the
              blue goalbox constants always match up with our goal.
        """
        # Does this matter for the goalie? It really shouldn't...
        if self.playerNumber == 1:
            self.resetLocTo(Constants.MIDFIELD_X,
                            Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                            Constants.HEADING_UP)
        elif self.playerNumber == 2:
            self.resetLocTo(Constants.BLUE_GOALBOX_MIDPOINT_X,
                            Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                            Constants.HEADING_DOWN)
        elif self.playerNumber == 3:
            self.resetLocTo(Constants.BLUE_GOALBOX_MIDPOINT_X,
                            Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                            Constants.HEADING_UP)
        elif self.playerNumber == 4:
            self.resetLocTo(Constants.BLUE_GOALBOX_CROSS_MIDPOINT_X,
                            Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                            Constants.HEADING_DOWN)
        elif self.playerNumber == 5:
            self.resetLocTo(Constants.BLUE_GOALBOX_CROSS_MIDPOINT_X,
                            Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                            Constants.HEADING_UP)


    #@todo: HACK HACK HACK Mexico 2012 to make sure we still re-converge properly even if
    #we get manually positioned
    #should make this nicer (or at least the locations)
    # Broken as of 4/2013
    def resetSetLocalization(self):
        gameSetResetUncertainties = _localization.LocNormalParams(50, 200, 1.0)

        if self.gameController.teamColor == Constants.teamColor.TEAM_BLUE:
            if self.playerNumber == 1:
                self.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
                                Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                Constants.HEADING_UP)
                if self.gameController.ownKickOff:
                    self.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                    Constants.CENTER_FIELD_Y,
                                    0,
                                    gameSetResetUncertainties)
                else:
                    self.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
                                    Constants.CENTER_FIELD_Y,
                                    0,
                                    gameSetResetUncertainties)
            # HACK: Figure out what this is supposed to do!
            #self.loc.resetLocToSide(True)
        else:
            if self.gameController.ownKickOff:
                self.resetLocTo(Constants.LANDMARK_YELLOW_GOAL_CROSS_X,
                                Constants.CENTER_FIELD_Y,
                                180,
                                gameSetResetUncertainties)
            else:
                self.resetLocTo(Constants.YELLOW_GOALBOX_LEFT_X,
                                Constants.CENTER_FIELD_Y,
                                180,
                                gameSetResetUncertainties)
            #self.loc.resetLocToSide(False)

    def checkSetLocalization(self):
        """
        Use during the first frame of the set state.
        If we think we are on the opponent's side of the field, either
            1) We didn't make it back and will be manually positioned or
            2) Our loc is wrong, and we could be anywhere.
        Reset to our own field cross. The loc system should be able to
        recover from there with high probability. 6/13/13
        """
        if self.loc.x > Constants.MIDFIELD_X:
            self.resetLocTo(Constants.LANDMARK_MY_FIELD_CROSS[0],
                            Constants.LANDMARK_MY_FIELD_CROSS[1],
                            Constants.HEADING_RIGHT)

    def resetLocalizationFromPenalty(self, top):
        """
        Resets localization from penalty.
        @param top: true is our goal is to our right, false if it is to our left.
        """
        if top:
            self.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                            Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                            Constants.HEADING_DOWN)
        else:
            self.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                            Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                            Constants.HEADING_UP)

    def resetGoalieLocalization(self):
        """
        Resets the goalie's localization to the manual position in the goalbox.
        """
        self.resetLocTo(Constants.FIELD_WHITE_LEFT_SIDELINE_X,
                        Constants.MIDFIELD_Y,
                        Constants.HEADING_RIGHT)

    def resetPenaltyKickLocalization(self):
        self.resetLocTo(Constants.LANDMARK_OPP_FIELD_CROSS[0] - 1.0,
                        Constants.MIDFIELD_Y,
                        Constants.HEADING_RIGHT)

    # THIS IS A HACK!
    # ... but until we have a world contextor or some such, it's a necessary one.

    def flipLocFilter(self):
        """
        Check where the goalie sees the ball and where we do.
        Record if we're generally correct, or if our flipped location
        is generally correct, or if neither one agrees with the goalie.
        NOTE: ignore whenever the ball is in the middle 2x2 meter box.
        """
        # Get goalie data
        for mate in self.teamMembers:
            if mate.isDefaultGoalie() and mate.active:
                if mate.ballOn and self.ball.vis.on:
                    # calculate global ball coordinates
                    # note: assume goalie is in center of goal.
                    goalie_x = Constants.FIELD_WHITE_LEFT_SIDELINE_X
                    goalie_y = Constants.MIDFIELD_Y

                    ball_x = goalie_x + (mate.ballDist * math.cos(mate.ballBearing))
                    ball_y = goalie_y + (mate.ballDist * math.sin(mate.ballBearing))
                    goalie_ball_location = Location(ball_x, ball_y)

                    # check against my data
                    my_ball_location = Location(self.ball.x, self.ball.y)
                    flipped_ball_location = Location(Constants.FIELD_GREEN_WIDTH - self.ball.x,
                                                     Constants.FIELD_GREEN_HEIGHT - self.ball.y)

                    if (mate.ballDist < 250 and
                        self.loc.x > Constants.MIDFIELD_X and
                        self.ball.x > Constants.MIDFIELD_X):
                        # I'm probably flipped!
                        self.updateFlipFilters(-1)

                        print "Goalie saw the ball close, and I think I and it are far."
                        print "Goalie sees ball at: " + str(goalie_ball_location)

                        break

                    if (goalie_ball_location.inCenterCenter() or
                        my_ball_location.inCenterCenter()):
                        # Ball is too close to the middle of the field. Risky.
                        self.updateFlipFilters(0)
                        break

                    if my_ball_location.distTo(goalie_ball_location) < 70:
                        # I'm probably in the right place!
                        self.updateFlipFilters(1)
                    elif flipped_ball_location.distTo(goalie_ball_location) < 70:
                        # I'm probably flipped!
                        self.updateFlipFilters(-1)
                    else:
                        # I don't agree with the goalie. Ignore.
                        self.updateFlipFilters(0)

        # If I've decided I should flip enough times, actually do it.
        if (len(self.flipFilter) == 10 and
            sum(self.flipFilter) > 6):
            self.flipLoc()
            # Reset filters! Don't want to flip again next frame.
            self.noFlipFilter = []
            self.flipFilter = []

    def updateFlipFilters(self, value):
        if value > 0:
            # I think I shouldn't flip
            self.noFlipFilter.append(1)
            self.flipFilter.append(0)
        elif value < 0:
            # I think I should flip
            self.noFlipFilter.append(0)
            self.flipFilter.append(1)
        else:
            # Neither option agrees with the goalie.
            self.noFlipFilter.append(0)
            self.flipFilter.append(0)

        # If filters are too long, pop oldest value.
        if len(self.noFlipFilter) > 10:
            self.noFlipFilter.pop(0)
        if len(self.flipFilter) > 10:
            self.flipFilter.pop(0)


    def flipLoc(self):
        """
        The goalie sees a ball.
        Check our current estimate of the ball against a flip.
        If the flip is much better, flip our loc.
        """

        print "According to the Goalie, I need to flip my loc!"

        print ("My position was (" + str(self.loc.x) + ", " + str(self.loc.y) + ", " + str(self.loc.h) +
               ") and the ball's position was " + str(self.ball.x) + ", " + str(self.ball.y) + ")")

        if (self.playerNumber == TeamMember.DEFAULT_GOALIE_NUMBER):
            # I am a goalie. Reset to the penatly box.
            print "I am a goalie. Resetting loc to the goalbox."
            self.resetGoalieLocalization()
            return

        reset_x = (-1*(self.loc.x - Constants.MIDFIELD_X)) + Constants.MIDFIELD_X
        reset_y = (-1*(self.loc.y - Constants.MIDFIELD_Y)) + Constants.MIDFIELD_Y
        reset_h = self.loc.h + 180
        if reset_h > 180:
            reset_h -= 360
        self.resetLocTo(reset_x, reset_y, reset_h)

        self.ownBallFilter = []
        self.ownBallFilterCount = 0
        self.dangerousBallFilter = []
        self.dangerousBallFilterCount = 0
