
import time

# Packages and modules from super-directories
from man import comm
from man import motion
from man import vision
from man.corpus import leds, sensors
from man.motion import MotionConstants
# Modules from this directory
from . import EKF
from . import GameController
from . import HeadTracking
from . import Navigator
from . import NaoOutput
from . import NogginConstants as Constants
from . import TypeDefs
# Packages and modules from sub-directories
from . import robots
from .playbook import GoTeam
from .players import Switch


DEBUG_OBJS = False
DEBUG_LOC = False

class Brain(object):
    """
    Class brings all of our components together and runs the behaviors
    """

    def __init__(self):
        """
        Class constructor
        """
	self.on = True
        # Setup nao modules inside brain for easy access
        #jf- self.nao = nao
        self.vision = vision.Vision()
	self.sensors = sensors.Sensors()
	self.comm = comm.inst
        self.comm.gc.team = 1
        # Initialize motion interface and module references
	self.motion = motion.MotionInterface()
        self.motionModule = motion

        # Retrieve our robot identification and set per-robot parameters
        self.CoA = robots.get_certificate()
            # coa = Certificate of Authenticity (to keep things short)
        self.comm.gc.player = self.CoA.player_number
        print self.CoA

        self.generator = self.temp_motion()

        # Initialize various components
        self.ekf = EKF.EKF(brain=self) # Localization Initialization
        self.my = TypeDefs.MyInfo()
        self.initFieldObjects()
        self.ball = TypeDefs.Ball(self.vision.ball)
        self.out = NaoOutput.NaoOutput(self) # Output Class

        self.player = Switch.selectedPlayer.SoccerPlayer(self)
        self.tracker = HeadTracking.HeadTracking(self)
        self.nav = Navigator.Navigator(self)
        self.playbook = GoTeam.GoTeam(self)
        self.gameController = GameController.GameController(self)

        #for taking pictures
        self.snapshot = False

        # Functional Variables

        #keyboard stuff
        #jf- nao.webots.enableKeyboard(NogginConstants.TIME_STEP)

        self.skipThisFrame = False # on the simulator, we skip every other frame

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

        self.bgCrossbar = TypeDefs.FieldObject(self.vision.bgCrossbar,
                                         Constants.VISION_BG_CROSSBAR)
        self.ygCrossbar = TypeDefs.FieldObject(self.vision.ygCrossbar,
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
    def temp_motion(self):
        for i in xrange(5):
            print "Generator frame: ", i
            yield

        print "Enqueueing test motion"
        self.motion.enqueue(motion.BodyJointCommand(3.0,
                                                    1,
                                                    [0., 0., 0., 0.],
                                                    0))
        while (True):
            yield

    def run(self):
	"""
        Main control loop called every TIME_STEP milliseconds
        """

        #print "Brain.run()"

        #print " IN Brain: ",self.sensors.angles

        #uncomment this to try temp_motion behavior
        #self.generator.next()
        # Update Environment
        self.ball.updateVision(self.vision.ball)
        self.updateFieldObjects()

        # Communications update
        self.updateComm()

        # Localization Update
        self.updateLocalization()

        #taking a picture if the space bar is pressed
        """
        js currently cant save frames on the nao
        if Constants.USE_SNAPSHOT:
            if self.snapshot:
                self.out.saveFrame()
                self.snapshot = False
        """

        # Behavior stuff
        self.gameController.run()
        self.player.run()
        self.tracker.run()
        self.nav.run()

        # Broadcast Report for Teammates
        self.setPacketData()

        # Update any logs we have
        #jf- self.out.updateLogs()

        if DEBUG_OBJS:
            #print "TeamColor: ",Constants.teamColorDict[self.my.teamColor]
            self.printObj(self.bglp,"BGLP")
            self.printObj(self.bgrp,"BGRP")
            self.printObj(self.yglp,"YGLP")
            self.printObj(self.ygrp,"YGRP")
#            self.printObj(self.ball,"BALL")

        if DEBUG_LOC:
            print "X,Y,H (%g,%g,%g)"%(self.my.x,self.my.y,self.my.h)

    def printObj(self,obj,name):
        if obj.dist > 0:
            print "Seen"+name+":"#, obj.visionId
            print "  width: ", obj.width
            print "  height: ", obj.height
            print "  distance: ", obj.dist
            print "  bearing: ", obj.bearing
            print "  x", obj.x
            print "  y", obj.y

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
        #print self.comm.latestComm()
        pass

    def updateLocalization(self):
        """
        Update estimates of robot and ball positions on the field
        """
        # UPDATE PHASE
        # Every frame we run the update phase based on odometry
        # Ball odometry is updated automatically
        self.ekf.updateOdometry((0.0,0.0,0.0))

        # CORRECTION PHASE
        # We correct the idea of our position based on objects we have seen
        # First we deal with standard field objects
        for fieldObject in self.myFieldObjects:
            if fieldObject.dist > 0:
                if Constants.DEBUG_FIELD_OBJECTS:
                    self.out.printf("Report object seen: %s\n" %
                                    (fieldObject.__str__(), ))
                    self.printLandmark(fieldObject)
                #print "Reporting field object ", fieldObject.visionId
                self.ekf.sawSpecificLandmark(fieldObject)
            else:
                if Constants.DEBUG_FIELD_OBJECTS:
                    print "Skipping field object ", fieldObject.visionId
        # We deal with corners last
        self.ekf.sawCorners(self.corners)

        # BALL CORRECTION PHASE
        # If we have seen the ball we correct our estimate of its location
        if self.ball.on:
            self.ekf.sawBall(self.ball.dist, -self.ball.bearing)
        # We report not seeing the ball to ensure that the velocity decays
        else:
            self.ekf.ballNotSeen()

        # Update global information to current estimates
        self.my.updateLoc(self.ekf)
        self.ball.updateLoc(self.ekf)

    def printLandmark(self, fo):
        print "Field Object ",fo.visionId," has:"
        print "\t bearing ", fo.bearing
        print "\t distance ", fo.dist
        print "\t field x", fo.x
        print "\t field y", fo.y

    # move to comm
    def setPacketData(self):
        # currently, teamNumber and playerNumber MUST be the first two values
        # passed to comm, whereas all the rest are Python-controlled.
        # eventually, all game-controller set info should be handled by Comm
        # alone, and extra Python stuff put in here
        self.comm.setData(self.my.x,
                          self.my.y,
                          self.my.h,
                          self.my.uncertX,
                          self.my.uncertY,
                          self.my.uncertH,
                          self.ball.x,
                          self.ball.y,
                          self.ball.uncertX,
                          self.ball.uncertY,
                          self.ball.dist,
                          self.playbook.currentSubRole,
                          -1) # Chase Time
