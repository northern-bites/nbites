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
from . import FallController
from . import Stability
from . import Leds
from . import robots

# Packages and modules from sub-directories
from .headTracking import HeadTracking
from .typeDefs import (Play, TeamMember)
from .navigator import Navigator
from .util import NaoOutput
from .playbook import PBInterface
from .players import Switch
from .kickDecider import KickDecider
import GameController

import _localization

from objects import (MyInfo, FieldObject)

# Import message protocol buffers
from .../share/messages import LedCommand_pb2

class Brain(object):
    """
    Class brings all of our components together and runs the behaviors
    """

    def __init__(self, arguments):
        """
        Class constructor
        """
        # Parse arguments
        self.playerNumber = arguments[0]
        self.teamNumber = arguments[1]

        self.counter = 0
        self.time = time.time()

        self.on = True
        # Output Class
        self.out = NaoOutput.NaoOutput(self)

        #initalize the leds
        #print leds
        self.leds = Leds.Leds(self)

        # Retrieve our robot identification and set per-robot parameters
        self.CoA = robots.get_certificate()
        self.CoA.setRobotGait(self.motion)

        # coa is Certificate of Authenticity (to keep things short)
        self.out.printf(self.CoA)
        self.out.printf("GC:  I am on team "+str(self.teamNumber))
        self.out.printf("GC:  I am player  "+str(self.playerNumber))

        # Initialize various components
        self.my = MyInfo(self.loc)

        # Functional Variables
        self.my.playerNumber = self.playerNumber
        self.my.teamColor = Constants.teamColor.TEAM_BLUE

        # Information about the environment
        # All field objects should come in as messages now
        #self.initFieldObjects()
        self.initTeamMembers()

        self.play = Play.Play()

        # Stability data
        self.stability = Stability.Stability(self.sensors)

        # FSAs
        self.player = Switch.selectedPlayer.SoccerPlayer(self)
        self.tracker = HeadTracking.HeadTracking(self)
        self.nav = Navigator.Navigator(self)
        self.playbook = PBInterface.PBInterface(self)
        self.kickDecider = KickDecider.KickDecider(self)
        self.gameController = GameController.GameController(self)
        self.fallController = FallController.FallController(self)

        # Messages
        self.inMesssages = {}
        self.outMessages = {}

    # def initFieldObjects(self):
    #     """
    #     Build our set of Field Objects which are team specific compared
    #     to the generic forms used in the vision system
    #     """
    #     # Build instances of the vision based field objects
    #     # Left post is on that goalie's left
    #     # Note: As of 6/8/12, ygrp holds info about ambiguous posts
    #     # Yellow goal left and right posts
    #     self.yglp = FieldObject(self.vision.yglp,
    #                             Constants.vis_landmark.VISION_YGLP,
    #                             self.loc)

    #     self.ygrp = FieldObject(self.vision.ygrp,
    #                             Constants.vis_landmark.VISION_YGRP,
    #                             self.loc)

    #     # Blue Goal left and right posts
    #     self.bglp = FieldObject(self.vision.bglp,
    #                             Constants.vis_landmark.VISION_BGLP,
    #                             self.loc)

    #     self.bgrp = FieldObject(self.vision.bgrp,
    #                             Constants.vis_landmark.VISION_BGRP,
    #                             self.loc)

    #     # Now we build the field objects to be based on our team color
    #     self.makeFieldObjectsRelative()

    # def makeFieldObjectsRelative(self):
    #     """
    #     Builds a list of fieldObjects based on their relative names to the robot
    #     Needs to be called when team color is determined
    #     """
    #     # Note: corner directions are relative to perspective of own goalie

    #     # Blue team setup
    #     if self.my.teamColor == Constants.teamColor.TEAM_BLUE:
    #         # Yellow goal
    #         self.oppGoalRightPost = self.yglp
    #         self.oppGoalLeftPost = self.ygrp
    #         # Blue Goal
    #         self.myGoalLeftPost = self.bglp
    #         self.myGoalRightPost = self.bgrp

    #     # Yellow team setup
    #     else:
    #         # Yellow goal
    #         self.myGoalLeftPost = self.yglp
    #         self.myGoalRightPost = self.ygrp
    #         # Blue Goal
    #         self.oppGoalRightPost = self.bglp
    #         self.oppGoalLeftPost = self.bgrp

    #     # Since, for ex.  bgrp points to the same things as myGoalLeftPost,
    #     # we can set these regardless of our team color
    #     self.myGoalLeftPost.associateWithRelativeLandmark(
    #         Constants.LANDMARK_MY_GOAL_LEFT_POST)
    #     self.myGoalRightPost.associateWithRelativeLandmark(
    #         Constants.LANDMARK_MY_GOAL_RIGHT_POST)
    #     self.oppGoalLeftPost.associateWithRelativeLandmark(
    #         Constants.LANDMARK_OPP_GOAL_LEFT_POST)
    #     self.oppGoalRightPost.associateWithRelativeLandmark(
    #         Constants.LANDMARK_OPP_GOAL_RIGHT_POST)

    #     # Build a list of all of the field objects with respect to team color
    #     self.myFieldObjects = [self.yglp, self.ygrp, self.bglp, self.bgrp]

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

    def run(self, msg1, msg2):
        """
        Main control loop called every TIME_STEP milliseconds
        """
        # Order here is very important
        # Update Environment
        self.time = time.time()

        # Communications update
        # Comm data should now come in messages
        #self.getCommUpdate()

        # Parse incoming messages
        self.inMessages['initialState'] = initialStateMessage.parseFromString(msg1) #deserialze first!
        self.inMessages['otherMessage'] = msg2

        # Update objects
        # TODO: update this functionality to get info from messages
        self.updateObjects()

        # Behavior stuff
        self.gameController.run()
        self.fallController.run()
        self.updatePlaybook()
        self.player.run()
        self.tracker.run()
        self.nav.run()

        #Set LED message
        self.leds.processLeds()

    # def getCommUpdate(self):
    #     for i in range(len(self.teamMembers)):
    #         mate = self.comm.teammate(i+1)
    #         self.teamMembers[i].update(mate)

        # Serialize outgoing messages and return a tuple of them
        self.outMessageList = list()
        for i in self.outMessages:
            self.outMessageList.append(i.SerializeToString())
        self.outMessageTuple = tuple(self.outMessageList)
        return self.outMessageTuple

    def updateObjects(self):
        """
        Update estimates of robot and ball positions on the field
        """
        self.ball.update()
        self.my.update()
        self.yglp.setBest()
        self.ygrp.setBest()
        self.bglp.setBest()
        self.bgrp.setBest()

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
        if self.my.teamColor == Constants.teamColor.TEAM_BLUE:
            if self.my.playerNumber == 1:
                self.loc.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.my.playerNumber == 2:
                self.loc.resetLocTo(Constants.BLUE_GOALBOX_RIGHT_X,
                                    Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    Constants.HEADING_DOWN,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.my.playerNumber == 3:
                self.loc.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                    Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    Constants.HEADING_DOWN,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.my.playerNumber == 4:
                self.loc.resetLocTo(Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
        else:
            if self.my.playerNumber == 1:
                self.loc.resetLocTo(Constants.YELLOW_GOALBOX_LEFT_X,
                                    Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    Constants.HEADING_DOWN,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.my.playerNumber == 2:
                self.loc.resetLocTo(Constants.YELLOW_GOALBOX_LEFT_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.my.playerNumber == 3:
                self.loc.resetLocTo(Constants.LANDMARK_YELLOW_GOAL_CROSS_X,
                                    Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    Constants.HEADING_UP,
                                    _localization.LocNormalParams(15.0, 15.0, 1.0))
            elif self.my.playerNumber == 4:
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

        if self.my.teamColor == Constants.teamColor.TEAM_BLUE:
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
        if self.my.teamColor == Constants.teamColor.TEAM_BLUE:
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
        if self.my.teamColor == Constants.teamColor.TEAM_BLUE:
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
