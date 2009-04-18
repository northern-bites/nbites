
# Redirect standard error to standard out
import sys
_stderr = sys.stderr
sys.stderr = sys.stdout
import time

# Packages and modules from super-directories
from man import comm
from man import motion
from man import vision
from man.corpus import leds
import sensors
from man.motion import MotionConstants
# Modules from this directory
from . import GameController
from . import FallController
from . import HeadTracking
from . import Navigator
from .util import NaoOutput
from . import NogginConstants as Constants
from . import TypeDefs
from . import Loc
from . import TeamConfig
# Packages and modules from sub-directories
from . import robots
#from .playbook import GoTeam
from .players import Switch

import _roboguardian


class Brain(object):
    """
    Class brings all of our components together and runs the behaviors
    """

    def __init__(self):
        """
        Class constructor
        """
        self.on = True
        # Output Class
        self.out = NaoOutput.NaoOutput(self)
        # Setup nao modules inside brain for easy access
        self.vision = vision.Vision()
        self.sensors = sensors.sensors
        self.comm = comm.inst
        self.comm.gc.team = TeamConfig.TEAM_NUMBER

        # Initialize motion interface and module references
        self.motion = motion.MotionInterface()
        self.motionModule = motion
        # Get the pointer to the C++ RoboGuardian object for use with Python
        self.roboguardian = _roboguardian.roboguardian
        self.roboguardian.enableFallProtection(True)
        # Get our reference to the C++ localization system
        self.loc = Loc()

        # Retrieve our robot identification and set per-robot parameters
        self.CoA = robots.get_certificate()
        self.CoA.setRobotGait(self.motion)

        # coa is Certificate of Authenticity (to keep things short)
        self.comm.gc.player = TeamConfig.PLAYER_NUMBER
        self.out.printf(self.CoA)
        self.out.printf("GC:  I am on team "+str(TeamConfig.TEAM_NUMBER))
        self.out.printf("GC:  I am player  "+str(TeamConfig.PLAYER_NUMBER))

        # Initialize various components
        self.my = TypeDefs.MyInfo()
        self.initFieldObjects()
        self.ball = TypeDefs.Ball(self.vision.ball)

        self.player = Switch.selectedPlayer.SoccerPlayer(self)
        self.tracker = HeadTracking.HeadTracking(self)
        self.nav = Navigator.Navigator(self)
        #self.playbook = GoTeam.GoTeam(self)
        self.gameController = GameController.GameController(self)
        self.fallController = FallController.FallController(self)

        # Functional Variables
        self.my.teamNumber = self.comm.gc.team
        self.my.playerNumber = self.comm.gc.player
        self.ownPlayers = [[0]*13]*4

    def initFieldObjects(self):
        """
        Build our set of Field Objects which are team specific compared
        to the generic forms used in the vision system
        """
        # Build instances of the vision based field objects
        # Yello goal left and right posts
        self.yglp = TypeDefs.FieldObject(self.vision.yglp,
                                         Constants.VISION_YGLP)
        self.ygrp = TypeDefs.FieldObject(self.vision.ygrp,
                                         Constants.VISION_YGRP)
        # Blue Goal left and right posts
        self.bglp = TypeDefs.FieldObject(self.vision.bglp,
                                         Constants.VISION_BGLP)
        self.bgrp = TypeDefs.FieldObject(self.vision.bgrp,
                                         Constants.VISION_BGRP)

        self.bgCrossbar = TypeDefs.Crossbar(self.vision.bgCrossbar,
                                            Constants.VISION_BG_CROSSBAR)
        self.ygCrossbar = TypeDefs.Crossbar(self.vision.ygCrossbar,
                                            Constants.VISION_YG_CROSSBAR)

        # Now we setup the corners
        self.corners = []
        self.lines = []

        # Now we build the field objects to be based on our team color
        self.makeFieldObjectsRelative()


    def makeFieldObjectsRelative(self):
        """
        Builds a list of fieldObjects based on their relative names to the robot
        Needs to be called when team color is determined
        """
        # Blue team setup
        if self.my.teamColor == Constants.TEAM_BLUE:
            # Yellow goal
            self.oppGoalRightPost = self.yglp
            self.oppGoalLeftPost = self.ygrp
            self.oppGoalCrossbar = self.ygCrossbar

            # Blue Goal
            self.myGoalLeftPost = self.bglp
            self.myGoalRightPost = self.bgrp
            self.myGoalCrossbar = self.bgCrossbar

        # Yellow team setup
        else:
            # Yellow goal
            self.myGoalLeftPost = self.yglp
            self.myGoalRightPost = self.ygrp
            self.myGoalCrossbar = self.ygCrossbar

            # Blue Goal
            self.oppGoalRightPost = self.bglp
            self.oppGoalLeftPost = self.bgrp
            self.oppGoalCrossbar = self.bgCrossbar

        # Since, for ex.  bgrp points to the same thins as myGoalLeftPost,
        # we can set these regardless of our team color
        self.myGoalLeftPost.associateWithRelativeLandmark(
                Constants.LANDMARK_MY_GOAL_LEFT_POST)
        self.myGoalRightPost.associateWithRelativeLandmark(
                Constants.LANDMARK_MY_GOAL_RIGHT_POST)
        self.oppGoalLeftPost.associateWithRelativeLandmark(
                Constants.LANDMARK_OPP_GOAL_LEFT_POST)
        self.oppGoalRightPost.associateWithRelativeLandmark(
                Constants.LANDMARK_OPP_GOAL_RIGHT_POST)

        # Build a list of all of the field objects with respect to team color
        self.myFieldObjects = [self.yglp, self.ygrp, self.bglp, self.bgrp]

##
##--------------CONTROL METHODS---------------##
##

    def run(self):
        """
        Main control loop called every TIME_STEP milliseconds
        """
        # Update Environment
        self.ball.updateVision(self.vision.ball)
        self.updateFieldObjects()

        # Communications update
        self.updateComm()

        # Localization Update
        self.updateLocalization()

        # Behavior stuff
        self.gameController.run()
        self.fallController.run()
        self.player.run()
        self.tracker.run()
        self.nav.run()

        # Broadcast Report for Teammates
        self.setPacketData()

        # Update any logs we have
        self.out.updateLogs()

    def updateFieldObjects(self):
        """
        Update information about seen objects
        """
        self.yglp.updateVision(self.vision.yglp)
        self.ygrp.updateVision(self.vision.ygrp)
        self.bglp.updateVision(self.vision.bglp)
        self.bgrp.updateVision(self.vision.bgrp)
        self.ygCrossbar.updateVision(self.vision.ygCrossbar)
        self.bgCrossbar.updateVision(self.vision.bgCrossbar)

        # Update the corner information
        self.corners = []
        if Constants.DEBUG_CORNERS and self.vision.fieldLines.numCorners > 0:
            self.out.printf("Number of corners: %g" %
                            (self.vision.fieldLines.numCorners,))

        for i, visionCorner in enumerate(self.vision.fieldLines.corners):
            # List contains more than just the corners
            if i >= self.vision.fieldLines.numCorners:
                break

            self.corners.append(TypeDefs.Corner(visionCorner))
            if Constants.DEBUG_CORNERS:
                self.out.printf("Corner number %d: %s\n" %
                                (i, self.corners[i].__str__()))

        # Now we get the latest list of lines
        self.lines = []
        if Constants.DEBUG_LINES and self.vision.fieldLines.numLines > 0:
            self.out.printf("Number of lines: %g" %
                            (self.vision.fieldLines.numLines,))

        for i, visionLine in enumerate(self.vision.fieldLines.lines):
            # List contains more than just the field lines
            if i >= self.vision.fieldLines.numLines:
                break

            # Add our line to the list
            self.lines.append(TypeDefs.Line(visionLine))
            if Constants.DEBUG_LINES:
                self.out.printf("Line numer %d: %s\n" %
                                (i+1, self.lines[i].__str__(),))

    def updateComm(self):
        temp = self.comm.latestComm()
        if len(temp) > 0 and len(temp[0])==17:
            self.out.printf(temp[0])
            self.packet = TypeDefs.Packet(temp[0])
            if self.packet.playerNumber != self.my.playerNumber:
                self.playbook.teammates[self.packet.playerNumber-1].update(self.packet)

    def updateLocalization(self):
        """
        Update estimates of robot and ball positions on the field
        """
        # Update global information to current estimates
        self.my.updateLoc(self.loc)
        self.ball.updateLoc(self.loc, self.my.teamColor)

    # move to comm
    def setPacketData(self):
        # currently, teamNumber, playerNumber, team color MUST be the first
        # values passed to comm, whereas all the rest are Python-controlled.
        # eventually, all game-controller set info should be handled by Comm
        # alone, and extra Python stuff put in here
        self.comm.setData(self.loc.x,
                          self.loc.y,
                          self.loc.h,
                          self.loc.xUncert,
                          self.loc.yUncert,
                          self.loc.hUncert,
                          self.loc.ballX,
                          self.loc.ballY,
                          self.loc.ballXUncert,
                          self.loc.ballYUncert,
                          self.ball.dist,
                          self.playbook.role,
                          self.playbook.currentSubRole,
                          self.playbook.me.chaseTime) # Chase Time
