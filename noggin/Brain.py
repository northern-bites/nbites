
# Redirect standard error to standard out
import sys
_stderr = sys.stderr
sys.stderr = sys.stdout

# Packages and modules from super-directories
from man import comm
from man import motion
from man import vision
#from man.corpus import leds
import sensors

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
from . import Leds
# Packages and modules from sub-directories
from . import robots
from .playbook import GoTeam
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
        self.profileFrames = 0
        self.on = True
        # Output Class
        self.out = NaoOutput.NaoOutput(self)
        # Setup nao modules inside brain for easy access
        self.vision = vision.Vision()
        self.sensors = sensors.sensors
        self.comm = comm.inst
        self.comm.gc.team = TeamConfig.TEAM_NUMBER

        #initalize the leds
        #print leds
        self.leds = Leds.Leds(self)

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
        # Functional Variables
        self.my.teamNumber = self.comm.gc.team
        self.my.playerNumber = self.comm.gc.player
        # Information about the environment
        self.initFieldObjects()
        self.ball = TypeDefs.Ball(self.vision.ball)
        self.sonar = TypeDefs.Sonar()
        # workaround for slarti (now trillian) sonar problems
        if self.CoA.name == 'marvin':
            self.sonar.MIN_DIST = 30.0

        # FSAs
        self.player = Switch.selectedPlayer.SoccerPlayer(self)
        self.tracker = HeadTracking.HeadTracking(self)
        self.nav = Navigator.Navigator(self)
        self.playbook = GoTeam.GoTeam(self)
        self.gameController = GameController.GameController(self)
        self.fallController = FallController.FallController(self)

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

    def profileRun(self):
        NUM_PROF_FRAMES = 1000
        if profileFrames == NUM_PROF_FRAMES:
            for x in xrange(NUM_PROF_FRAMES):
                if x == 0:
                    allStats = pstats.Stats('stats'+str(x))
                else :
                    allStats.add('stats'+str(x))
            allStats.dump_stats('totalRunStats')

        else :
            cProfile.run('run()','stats'+str(profileFrames))
            profileFrames += 1

    def run(self):
        """
        Main control loop called every TIME_STEP milliseconds
        """
        # Update Environment
        self.ball.updateVision(self.vision.ball)
        self.updateFieldObjects()
        self.sonar.updateSensors(self.sensors,sensors.UltraSoundMode)

        # Communications update
        self.updateComm()

        # Localization Update
        self.updateLocalization()

        #Set LEDS
        self.leds.processLeds()

        # Behavior stuff
        self.gameController.run()
        self.fallController.run()
        self.playbook.run()
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

        # Now we get the latest list of lines
        self.lines = []

    def updateComm(self):
        temp = self.comm.latestComm()
        for packet in temp:
            if len(packet) == Constants.NUM_PACKET_ELEMENTS:
                packet = TypeDefs.Packet(packet)
                if packet.playerNumber != self.my.playerNumber:
                    self.playbook.update(packet)

    def updateLocalization(self):
        """
        Update estimates of robot and ball positions on the field
        """
        # Update global information to current estimates
        self.my.updateLoc(self.loc)
        self.ball.updateLoc(self.loc, self.my)

    # move to comm
    def setPacketData(self):
        # Team color, team number, and player number are all appended to this
        # list by the underlying comm module implemented in C++
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
                          self.playbook.determineChaseTime(),
                          self.loc.ballVelX,
                          self.loc.ballVelY)

    def resetLocalization(self):
        """
        Reset our localization
        """
        if self.out.loggingLoc:
            self.out.stopLocLog()
            self.out.startLocLog()
        self.loc.reset()

    def resetGoalieLocalization(self):
        """
        Reset our localization
        """
        if self.out.loggingLoc:
            self.out.stopLocLog()
            self.out.startLocLog()
        if self.my.teamColor == Constants.TEAM_BLUE:
            self.loc.blueGoalieReset()
        else:
            self.loc.redGoalieReset()
