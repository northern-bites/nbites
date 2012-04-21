import time
import sys

# Redirect standard error to standard out
_stderr = sys.stderr
sys.stderr = sys.stdout
## import cProfile
## import pstats

# Packages and modules from super-directories
from man import comm
from man import motion
import vision
#from man.corpus import leds
import sensors
import noggin_constants as Constants
import loggingBoard

# Modules from this directory
from . import GameController
from . import FallController
from . import Stability
from . import Loc
from . import TeamConfig
from . import Leds
from . import robots

# Packages and modules from sub-directories
from .headTracking import HeadTracking
from .typeDefs import (Sonar, Packet,
                       Play, TeamMember)
from .navigator import Navigator
from .util import NaoOutput
from .playbook import PBInterface
from .players import Switch
from .kickDecider import KickDecider

import _roboguardian
import _speech

from objects import (MyInfo, FieldObject, Ball)

class Brain(object):
    """
    Class brings all of our components together and runs the behaviors
    """

    def __init__(self):
        """
        Class constructor
        """
        self.counter = 0
        self.time = time.time()

        self.on = True
        # Output Class
        self.out = NaoOutput.NaoOutput(self)

        # Setup nao modules inside brain for easy access
        self.vision = vision.vision
        self.sensors = sensors.sensors
        self.logger = loggingBoard.loggingBoard
        self.comm = comm.inst
        self.comm.gc.team = TeamConfig.TEAM_NUMBER
        self.comm.gc.player = TeamConfig.PLAYER_NUMBER

        #initalize the leds
        #print leds
        self.leds = Leds.Leds(self)
        self.speech = _speech.speech

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
        self.out.printf(self.CoA)
        self.out.printf("GC:  I am on team "+str(TeamConfig.TEAM_NUMBER))
        self.out.printf("GC:  I am player  "+str(TeamConfig.PLAYER_NUMBER))

        # Initialize various components
        self.my = MyInfo(self.loc)

        # Functional Variables
        self.my.playerNumber = self.comm.gc.player
        if self.comm.gc.color == GameController.TEAM_BLUE:
            self.my.teamColor = Constants.teamColor.TEAM_BLUE
        else:
            self.my.teamColor = Constants.teamColor.TEAM_RED

        # Information about the environment
        self.initFieldObjects()
        self.initTeamMembers()
        self.ball = Ball(self.vision.ball, self.loc, self.my)

        self.play = Play.Play()
        self.sonar = Sonar.Sonar()
        if Constants.LOG_COMM:
            self.out.startCommLog()

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

    def initFieldObjects(self):
        """
        Build our set of Field Objects which are team specific compared
        to the generic forms used in the vision system
        """
        # Build instances of the vision based field objects
        # Left post is on that goalie's left
        # Yellow goal left and right posts
        self.yglp = FieldObject(self.vision.yglp,
                                Constants.vis_landmark.VISION_YGLP,
                                self.loc)

        self.ygrp = FieldObject(self.vision.ygrp,
                                Constants.vis_landmark.VISION_YGRP,
                                self.loc)

        # Blue Goal left and right posts
        self.bglp = FieldObject(self.vision.bglp,
                                Constants.vis_landmark.VISION_BGLP,
                                self.loc)

        self.bgrp = FieldObject(self.vision.bgrp,
                                Constants.vis_landmark.VISION_BGRP,
                                self.loc)

        # Now we build the field objects to be based on our team color
        self.makeFieldObjectsRelative()

    def makeFieldObjectsRelative(self):
        """
        Builds a list of fieldObjects based on their relative names to the robot
        Needs to be called when team color is determined
        """
        # Note: corner directions are relative to perspective of own goalie

        # Blue team setup
        if self.my.teamColor == Constants.teamColor.TEAM_BLUE:
            # Yellow goal
            self.oppGoalRightPost = self.yglp
            self.oppGoalLeftPost = self.ygrp
            # Blue Goal
            self.myGoalLeftPost = self.bglp
            self.myGoalRightPost = self.bgrp

        # Yellow team setup
        else:
            # Yellow goal
            self.myGoalLeftPost = self.yglp
            self.myGoalRightPost = self.ygrp
            # Blue Goal
            self.oppGoalRightPost = self.bglp
            self.oppGoalLeftPost = self.bgrp

        # Since, for ex.  bgrp points to the same things as myGoalLeftPost,
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
        # order here is very important
        # Update Environment
        self.time = time.time()
        self.sonar.updateSensors(self.sensors)

        # Communications update
        self.updateComm()

        # Update objects
        self.updateObjects()

        #Set LEDS
        self.leds.processLeds()

        # Behavior stuff
        self.gameController.run()
        self.fallController.run()
        self.updatePlaybook()
        self.player.run()
        self.tracker.run()
        self.nav.run()

        # Broadcast Report for Teammates
        self.setPacketData()

        # Update any logs we have
        self.out.updateLogs()

    def updateComm(self):
        temp = self.comm.latestComm()
        for packet in temp:
            if len(packet) == Constants.NUM_PACKET_ELEMENTS:
                packet = Packet.Packet(packet)
                if packet.playerNumber != self.my.playerNumber:
                    self.teamMembers[packet.playerNumber-1].update(packet)
                if Constants.LOG_COMM:
                    self.out.logRComm(packet)
        # update the activity of our teammates here
        # active field is set to true upon recipt of a new packet.
        for mate in self.teamMembers:
            if (mate.active and mate.isDead()):
                mate.active = False
                if self.my.playerNumber == 1:
                    return
                if mate.playerNumber == 1:
                    print "WARNING: THE GOALIE IS DEAD."
                    return

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
    def setPacketData(self):
        # Team color, team number, and player number are all appended to this
        # list by the underlying comm module implemented in C++
        loc = self.loc
        self.comm.setData(#loc.x,
                          #loc.y,
                          #loc.h,
                          #loc.xUncert,
                          #loc.yUncert,
                          #loc.hUncert,
                          #loc.ballX,
                          #loc.ballY,
                          #loc.ballXUncert,
                          #loc.ballYUncert,
                          self.ball.vis.dist,
                          self.ball.vis.bearing,
                          self.ball.vis.on,
                          self.ball.vis.framesOn,
                          #self.play.role,
                          #self.play.subRole,
                          self.teamMembers[self.my.playerNumber].chaseTime)
                          #loc.ballVelX,
                          #loc.ballVelY)

        # TODO: remove this and log through C++ and the Logger instead.
        if Constants.LOG_COMM:
            packet = Packet.Packet((TeamConfig.TEAM_NUMBER,
                                    TeamConfig.PLAYER_NUMBER,
                                    self.my.teamColor,
                                    loc.x,
                                    loc.y,
                                    loc.h,
                                    loc.xUncert,
                                    loc.yUncert,
                                    loc.hUncert,
                                    loc.ballX,
                                    loc.ballY,
                                    loc.ballXUncert,
                                    loc.ballYUncert,
                                    self.ball.dist,
                                    self.ball.bearing,
                                    self.play.role,
                                    self.play.subRole,
                                    self.playbook.pb.me.chaseTime,
                                    loc.ballVelX,
                                    loc.ballVelY))
            self.out.logSComm(packet)

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
        if self.my.teamColor == Constants.teamColor.TEAM_BLUE:
            self.loc.blueGoalieReset()
        else:
            self.loc.redGoalieReset()
