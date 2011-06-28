
# Redirect standard error to standard out
import time
import sys
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

# Modules from this directory
from . import GameController
from . import FallController
from . import Stability
from . import NogginConstants as Constants
from . import Loc
from . import TeamConfig
from . import Leds
from . import robots
# Packages and modules from sub-directories
from .headTracking import HeadTracking
from .typeDefs import (MyInfo, Ball, Landmarks, Sonar, Packet,
                       Play, TeamMember)
from .navigator import Navigator
from .util import NaoOutput
from .playbook import PBInterface
from .players import Switch
from .kickDecider import KickDecider

import _roboguardian
import _speech

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
        self.my = MyInfo.MyInfo()

        # Functional Variables
        self.my.playerNumber = self.comm.gc.player

        # Information about the environment
        self.initFieldObjects()
        self.initTeamMembers()
        self.ball = Ball.Ball(self.vision.ball)
        self.play = Play.Play()
        self.sonar = Sonar.Sonar()

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
        self.yglp = Landmarks.FieldObject(self.vision.yglp,
                                          Constants.VISION_YGLP)
        self.ygrp = Landmarks.FieldObject(self.vision.ygrp,
                                          Constants.VISION_YGRP)

        # Blue Goal left and right posts
        self.bglp = Landmarks.FieldObject(self.vision.bglp,
                                          Constants.VISION_BGLP)
        self.bgrp = Landmarks.FieldObject(self.vision.bgrp,
                                          Constants.VISION_BGRP)

        # Add field corners
        self.corners = []
        for i in range(17): # See PyVision.cpp for index meanings, 15-31
            self.corners.append(Landmarks.FieldCorner(i))

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
            # Blue Goal
            self.myGoalLeftPost = self.bglp
            self.myGoalRightPost = self.bgrp
            # Corners
            self.myHalfLeftCorner = self.corners[0]
            self.myHalfRightCorner = self.corners[1]
            self.myBoxLeftT = self.corners[2]
            self.myBoxRightT = self.corners[3]
            self.myBoxLeftL = self.corners[4]
            self.myBoxRightL = self.corners[5]
            self.centerLeftT = self.corners[7]
            self.centerRightT = self.corners[8]
            self.centerLeftCross = self.corners[15]
            self.centerRightCross = self.corners[16]
            self.oppHalfLeftCorner = self.corners[9]
            self.oppHalfRightCorner = self.corners[8]
            self.oppBoxLeftT = self.corners[11]
            self.oppBoxRightT = self.corners[10]
            self.oppBoxLeftL = self.corners[13]
            self.oppBoxRightL = self.corners[12]

        # Yellow team setup
        else:
            # Yellow goal
            self.myGoalLeftPost = self.yglp
            self.myGoalRightPost = self.ygrp
            # Blue Goal
            self.oppGoalRightPost = self.bglp
            self.oppGoalLeftPost = self.bgrp
            # Corners
            self.myHalfLeftCorner = self.corners[8]
            self.myHalfRightCorner = self.corners[9]
            self.myBoxLeftT = self.corners[10]
            self.myBoxRightT = self.corners[11]
            self.myBoxLeftL = self.corners[12]
            self.myBoxRightL = self.corners[13]
            self.centerLeftT = self.corners[8]
            self.centerRightT = self.corners[7]
            self.centerLeftCross = self.corners[16]
            self.centerRightCross = self.corners[15]
            self.oppHalfLeftCorner = self.corners[1]
            self.oppHalfRightCorner = self.corners[0]
            self.oppBoxLeftT = self.corners[3]
            self.oppBoxRightT = self.corners[2]
            self.oppBoxLeftL = self.corners[5]
            self.oppBoxRightL = self.corners[4]

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

        # Set x and y values for corners
        self.myHalfLeftCorner.associateWithRelativeLandmark(
            Constants.LANDMARK_MY_CORNER_LEFT_L)
        self.myHalfRightCorner.associateWithRelativeLandmark(
            Constants.LANDMARK_MY_CORNER_RIGHT_L)
        self.myBoxLeftT.associateWithRelativeLandmark(
            Constants.LANDMARK_MY_GOAL_LEFT_T)
        self.myBoxRightT.associateWithRelativeLandmark(
            Constants.LANDMARK_MY_GOAL_RIGHT_T)
        self.myBoxLeftL.associateWithRelativeLandmark(
            Constants.LANDMARK_MY_GOAL_LEFT_L)
        self.myBoxRightL.associateWithRelativeLandmark(
            Constants.LANDMARK_MY_GOAL_RIGHT_L)
        self.centerLeftT.associateWithRelativeLandmark(
            Constants.LANDMARK_CENTER_LEFT_T)
        self.centerRightT.associateWithRelativeLandmark(
            Constants.LANDMARK_CENTER_RIGHT_T)
        self.centerLeftCross.associateWithRelativeLandmark(
            Constants.LANDMARK_CENTER_LEFT_CROSS)
        self.centerRightCross.associateWithRelativeLandmark(
            Constants.LANDMARK_CENTER_RIGHT_CROSS)
        self.oppHalfLeftCorner.associateWithRelativeLandmark(
            Constants.LANDMARK_OPP_CORNER_LEFT_L)
        self.oppHalfRightCorner.associateWithRelativeLandmark(
            Constants.LANDMARK_OPP_CORNER_RIGHT_L)
        self.oppBoxLeftT.associateWithRelativeLandmark(
            Constants.LANDMARK_OPP_GOAL_LEFT_T)
        self.oppBoxRightT.associateWithRelativeLandmark(
            Constants.LANDMARK_OPP_GOAL_RIGHT_T)
        self.oppBoxLeftL.associateWithRelativeLandmark(
            Constants.LANDMARK_OPP_GOAL_LEFT_L)
        self.oppBoxRightL.associateWithRelativeLandmark(
            Constants.LANDMARK_OPP_GOAL_RIGHT_L)

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
        self.updateVisualObjects()


        # ** # debugging
        #print "visual update:"
        #print "yglp:",self.yglp.visDist,self.yglp.visBearing
        #print "ygrp:",self.ygrp.visDist,self.ygrp.visBearing
        # ** #


        self.sonar.updateSensors(self.sensors)

        # Communications update
        self.updateComm()

        # Localization Update
        self.updateLocalization()

        # ** # debugging
        #print "loc update:"
        #print "yglp:",self.yglp.locDist,self.yglp.locBearing
        #print "ygrp:",self.ygrp.locDist,self.ygrp.locBearing
        # ** #

        # Choose whether we use Vision or Localization
        self.updateBestValues()

        # ** # debugging
        #print "best values:"
        #print "yglp:",self.yglp.dist,self.yglp.bearing
        #print "ygrp:",self.ygrp.dist,self.ygrp.bearing
        #print "rel values:"
        #print "yglp:",self.yglp.relX,self.yglp.relY
        #print "ygrp:",self.ygrp.relX,self.ygrp.relY
        # ** #

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

    def updateVisualObjects(self):
        """
        Update information about seen objects
        """
        self.ball.updateVision(self.vision.ball)

        self.yglp.updateVision(self.vision.yglp)
        self.ygrp.updateVision(self.vision.ygrp)
        self.bglp.updateVision(self.vision.bglp)
        self.bgrp.updateVision(self.vision.bgrp)

        # If there are ANY possible corners in vision, set values for
        # ALL corners.
        for c in self.vision.fieldLines.corners:
            for i in range(len(self.corners)):
                self.corners[i].setVisualCorner(c)

        # If a possible visualCorner is definite, set it's visual values
        # to the correct FieldCorner object.
        for c in self.vision.fieldLines.corners:
            for p in c.possibilities:
                if p > 14:
                    self.corners[p-15].setVisualCorner(c)
        # Check all FieldCorners and reset values if not in this frame.
        for i in range(len(self.corners)):
            self.corners[i].updateVision()

        self.time = time.time()

    def updateComm(self):
        temp = self.comm.latestComm()
        for packet in temp:
            if len(packet) == Constants.NUM_PACKET_ELEMENTS:
                packet = Packet.Packet(packet)
                if packet.playerNumber != self.my.playerNumber:
                    self.teamMembers[packet.playerNumber-1].update(packet)
        # update the activity of our teammates here
        # active field is set to true upon recipt of a new packet.
        for mate in self.teamMembers:
            if (mate.active and mate.isDead()):
                mate.active = False

    def updateLocalization(self):
        """
        Update estimates of robot and ball positions on the field
        """
        self.my.updateLoc(self.loc)

        self.ball.updateLoc(self.loc, self.my)
        self.yglp.updateLoc(self.loc, self.my)
        self.ygrp.updateLoc(self.loc, self.my)
        self.bglp.updateLoc(self.loc, self.my)
        self.bgrp.updateLoc(self.loc, self.my)

        for i in range(len(self.corners)):
            self.corners[i].updateLoc(self.loc, self.my)

    def updateBestValues(self):
        """
        Update estimates about objects using best information available
        """
        self.ball.updateBestValues(self.my)
        self.yglp.updateBestValues()
        self.ygrp.updateBestValues()
        self.bglp.updateBestValues()
        self.bgrp.updateBestValues()

        for i in range(len(self.corners)):
            self.corners[i].updateBestValues()

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
        self.comm.setData(loc.x,
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
                          loc.ballVelY)

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


