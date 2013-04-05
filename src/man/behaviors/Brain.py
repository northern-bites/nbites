import time
import sys

# Redirect standard error to standard out
_stderr = sys.stderr
sys.stderr = sys.stdout
## import cProfile
## import pstats

# Packages and modules from super-directories
import noggin_constants as Constants

# Modules from this directory
from . import Leds
#from . import robots
from . import Ball
from . import MyInfo
from . import GameController

# Packages and modules from sub-directories
from .headTracking import HeadTracking
from .typeDefs import (Play, TeamMember)
from .navigator import Navigator
from .util import NaoOutput
from .playbook import PBInterface
from .players import Switch
from .kickDecider import KickDecider

#from objects import (FieldObject)

# Import message protocol buffers and interface
import interface
import LedCommand_proto
import GameState_proto
import WorldModel_proto
import BallModel_proto
import PMotion_proto
import VisionRobot_proto
import VisionField_proto

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

        self.on = True
        # Output Class
        self.out = NaoOutput.NaoOutput(self)
        self.my = MyInfo.MyInfo()

        #initalize the leds and game controller
        self.leds = Leds.Leds(self)
        self.gameController = GameController.GameController(self)

        # Retrieve our robot identification and set per-robot parameters
        #self.CoA = robots.get_certificate()

        # coa is Certificate of Authenticity (to keep things short)
        #self.out.printf(self.CoA)
        #self.out.printf("GC:  I am on team "+str(self.teamNumber))
        #self.out.printf("GC:  I am player  "+str(self.playerNumber))

        # Information about the environment
        # All field objects should come in as messages now
        #self.initFieldObjects()
        self.ball = Ball.Ball()
        self.initTeamMembers()

        self.play = Play.Play()

        # FSAs
        self.player = Switch.selectedPlayer.SoccerPlayer(self)
        self.tracker = HeadTracking.HeadTracking(self)
        self.nav = Navigator.Navigator(self)
        self.playbook = PBInterface.PBInterface(self)
        self.kickDecider = KickDecider.KickDecider(self)

        # Message interface
        self.interface = interface.interface


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
        Main control loop called every TIME_STEP milliseconds
        """

        # Check if any messages are null.
        # self.out.printf("Check for null messages:")
        # self.out.printf("ledCommand:    ")
        # self.out.printf(self.interface.ledCommand)
        # self.out.printf("gameState:     ")
        # self.out.printf(self.interface.gameState)
        # self.out.printf("filteredBall:  ")
        # self.out.printf(self.interface.filteredBall)
        # self.out.printf("motionCommand: ")
        # self.out.printf(self.interface.motionCommand)

        # Order here is very important
        # Update Environment
        self.time = time.time()

        # Update objects
        self.updateVisionObjects()

        # Behavior stuff
        self.gameController.run()
        self.updatePlaybook()
        self.player.run()
        #self.tracker.run()
        self.nav.run()

        #Set LED message
        self.leds.processLeds()

    def getCommUpdate(self):
        # TODO: do this for more than one teamMember
        pass
        #for i in range(len(self.teamMembers)):
        #self.teamMembers[0].update(self.inMessages['worldModel'])

    def updateObjects(self):
        """
        Update estimates of robot and ball positions on the field
        """
        self.ball = self.brain.interface.visionBall
        self.yglp = self.brain.interface.visionField.goal_post_l.visual_detection
        self.ygrp = self.brain.interface.visionField.goal_post_r.visual_detection

    def updatePlaybook(self):
        """
        updates self.play to the new play
        """
        self.playbook.update(self.play)

    # move to comm
    # def setCommData(self):
    #     # Team color, team number, and player number are all appended to this
    #     # list by the underlying comm module implemented in C++
    #     loc = self.loc
    #     self.comm.setData(self.my.playerNumber,
    #                       self.play.role, self.play.subRole,
    #                       self.playbook.pb.me.chaseTime)

    # TODO: Take this out once new comm is in...
    # def activeTeamMates(self):
    #     activeMates = 0
    #     for i in xrange(Constants.NUM_PLAYERS_PER_TEAM):
    #         mate = self.teamMembers[i]
    #         if mate.active:
    #             activeMates += 1
    #     return activeMates

    def resetInitialLocalization(self):
        """
        Reset loc according to team number and team color.
        Note: Loc uses truly global coordinates.
        """
        if self.gameController.teamColor == Constants.teamColor.TEAM_BLUE:
            if self.playerNumber == 1:
                self.loc.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.playerNumber == 2:
                self.loc.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
                                    Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    Constants.HEADING_DOWN,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.playerNumber == 3:
                self.loc.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                    Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    Constants.HEADING_DOWN,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.playerNumber == 4:
                self.loc.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
        else:
            if self.playerNumber == 1:
                self.loc.resetLocTo(Constants.YELLOW_GOALBOX_LEFT_X,
                                    Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    Constants.HEADING_DOWN,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.playerNumber == 2:
                self.loc.resetLocTo(Constants.YELLOW_GOALBOX_LEFT_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.playerNumber == 3:
                self.loc.resetLocTo(Constants.LANDMARK_YELLOW_GOAL_CROSS_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.playerNumber == 4:
                self.loc.resetLocTo(Constants.LANDMARK_YELLOW_GOAL_CROSS_X,
                                    Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    Constants.HEADING_DOWN,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))

        # Loc knows the side of the field now. Reset accordingly.
        self.onOwnFieldSide = True

    #@todo: HACK HACK HACK Mexico 2012 to make sure we still re-converge properly even if
    #we get manually positioned
    #should make this nicer (or at least the locations)
    def resetSetLocalization(self):

        gameSetResetUncertainties = _localization.LocNormalParams(50, 200, 1.0)

        if self.gameController.teamColor == Constants.teamColor.TEAM_BLUE:
# #            if self.my.playerNumber == 1:
# #                self.loc.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
# #                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
# #                                    Constants.HEADING_UP)
#             if self.gameController.ownKickOff:
#                 self.loc.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
#                                     Constants.CENTER_FIELD_Y,
#                                     0,
#                                     gameSetResetUncertainties)
#             else:
#                 self.loc.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
#                                     Constants.CENTER_FIELD_Y,
#                                     0,
#                                     gameSetResetUncertainties)
            self.loc.resetLocToSide(True)
        else:
            # if self.gameController.ownKickOff:
            #     self.loc.resetLocTo(Constants.LANDMARK_YELLOW_GOAL_CROSS_X,
            #                         Constants.CENTER_FIELD_Y,
            #                         180,
            #                         gameSetResetUncertainties)
            # else:
            #     self.loc.resetLocTo(Constants.YELLOW_GOALBOX_LEFT_X,
            #                         Constants.CENTER_FIELD_Y,
            #                         180,
            #                         gameSetResetUncertainties)
            self.loc.resetLocToSide(False)

    def resetLocalizationFromPenalty(self):
        """
        Resets localization to both possible locations, depending on team color.
        """
        if self.gameController.teamColor == Constants.teamColor.TEAM_BLUE:
            self.loc.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                Constants.HEADING_UP,
                                Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                Constants.HEADING_DOWN,
                                _localization.LocNormalParams(15.0, 15.0, 1.0),
                                _localization.LocNormalParams(15.0, 15.0, 1.0))
        else:
            self.loc.resetLocTo(Constants.LANDMARK_YELLOW_GOAL_CROSS_X,
                                Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                Constants.HEADING_UP,
                                Constants.LANDMARK_YELLOW_GOAL_CROSS_X,
                                Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                Constants.HEADING_DOWN,
                                _localization.LocNormalParams(15.0, 15.0, 1.0),
                                _localization.LocNormalParams(15.0, 15.0, 1.0))

        # Loc knows the side of the field now. Reset accordingly.
        self.onOwnFieldSide = True

    def resetGoalieLocalization(self):
        """
        Resets the goalie's localization to the manual position in the goalbox.
        """
        if self.gameController.teamColor == Constants.teamColor.TEAM_BLUE:
            self.loc.resetLocTo(Constants.FIELD_WHITE_LEFT_SIDELINE_X,
                                Constants.MIDFIELD_Y,
                                Constants.HEADING_RIGHT,
                                _localization.LocNormalParams(15.0, 15.0, 1.0))
        else:
            self.loc.resetLocTo(Constants.FIELD_WHITE_RIGHT_SIDELINE_X,
                                Constants.MIDFIELD_Y,
                                Constants.HEADING_LEFT,
                                _localization.LocNormalParams(15.0, 15.0, 1.0))

        # Loc knows the side of the field now. Reset accordingly.
        self.onOwnFieldSide = True

    #TODO: write this method!
    def resetPenaltyKickLocalization(self):
        pass
