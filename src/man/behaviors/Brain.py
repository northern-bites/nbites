import time
import sys
import math

# Redirect standard error to standard out
_stderr = sys.stderr
sys.stderr = sys.stdout

# Import message protocol buffers and interface
import interface
import LedCommand_proto
import GameState_proto
import WorldModel_proto
import RobotLocation_proto
import PMotion_proto
import MotionStatus_proto
import ButtonState_proto
import FallStatus_proto
import StiffnessControl_proto
import Vision_proto

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

        self.interface = interface.interface

        print "Brain initialized"

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
        

        # Update Environment
        self.time = time.time()
        self.counter += 1

        if self.counter % 10 == 0:
            print(self.interface.vision)
        
        # Update objects
        # self.updateVisionObjects()
        # self.updateObstacles()
        # self.updateMotion()
        # self.updateLoc()
        # self.getCommUpdate()

        # for new vision stuff
        # self.updateVision()

        # Set myWorldModel for Comm
        # self.updateComm()

        # Flush the output
        sys.stdout.flush()

        # print("Counter: ", self.counter)



    def updateComm(self):
        me = self.teamMembers[self.playerNumber - 1]
        output = self.interface.myWorldModel

        output.timestamp = int(self.time * 1000)
        output.my_x = me.x
        output.my_y = me.y
        output.my_h = me.h
        output.my_uncert = me.locUncert
        output.walking_to_x = me.walkingToX
        output.walking_to_y = me.walkingToY
        output.ball_on = me.ballOn
        output.ball_age = me.ballAge
        output.ball_dist = me.ballDist
        output.ball_bearing = me.ballBearing
        output.ball_vel_x = me.ballVelX
        output.ball_vel_y = me.ballVelY
        output.ball_uncert = me.ballUncert
        output.role = me.role
        output.in_kicking_state = me.inKickingState
        if me.inKickingState:
            output.kicking_to_x = me.kickingToX
            output.kicking_to_y = me.kickingToY
        output.fallen = me.fallen
        output.active = me.active
        output.claimed_ball = me.claimedBall

    def getCommUpdate(self):
        self.teamMembers[self.playerNumber - 1].updateMe()
        self.game = self.interface.gameState
        
        if self.game.have_remote_gc:
            for i in range(self.game.team_size()):
                if self.game.team(i).team_number == self.teamNumber: 
                    self.ourScore = self.game.team(i).score
                else:
                    self.theirScore = self.game.team(i).score

        for i in range(len(self.teamMembers)):
            if (i == self.playerNumber - 1):
                continue
            self.teamMembers[i].update(self.interface.worldModelList()[i])

    def updateMotion(self):
        self.motion = self.interface.motionStatus

    def updateVision(self):
        self.visionLines = self.interface.vision.line
        self.visionCorners = self.interface.vision.corner
        self.visionCircle = self.interface.vision.circle
        self.vision = self.interface.vision

    def updateVisionObjects(self):
        """
        Update estimates of robot and ball positions on the field
        """
        self.ball = self.interface.filteredBall
        self.sharedBall = self.interface.sharedBall
        # if (self.player.gameState == 'gameReady'
        #     or self.player.gameState == 'gameSet'):
        #     self.ball.x = Constants.CENTER_FIELD_X
        #     self.ball.y = Constants.CENTER_FIELD_Y
        self.naiveBall = self.interface.naiveBall

    def updateObstacles(self):
        self.obstacles = [0.] * 9
        self.obstacleDetectors = ['n'] * 9
        size = self.interface.fieldObstacles.obstacle_size()
        for i in range(size):
            curr_obst = self.interface.fieldObstacles.obstacle(i)
            if curr_obst.position != curr_obst.position.NONE:
                self.obstacles[int(curr_obst.position)] = (curr_obst.distance, curr_obst.closest_y)
                if curr_obst.detector == curr_obst.detector.ARMS:
                    self.obstacleDetectors[int(curr_obst.position)] = 'a'
                elif curr_obst.detector == curr_obst.detector.SONARS:
                    self.obstacleDetectors[int(curr_obst.position)] = 's'
                elif curr_obst.detector == curr_obst.detector.VISION:
                    self.obstacleDetectors[int(curr_obst.position)] = 'v'
                else:
                    self.obstacleDetectors[int(curr_obst.position)] = 'n'

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
        # self.loc = RobotLocation(self.interface.loc.x,
        #                          self.interface.loc.y,
        #                          self.interface.loc.h * (180. / math.pi))
        # self.locUncert = self.interface.loc.uncert
        # self.lost = self.interface.loc.lost

    def resetLocTo(self, x, y, h):
        """
        Sends a reset request to loc to reset to given x, y, h
        """
        self.interface.resetLocRequest.x = x
        self.interface.resetLocRequest.y = y
        self.interface.resetLocRequest.h = h * (math.pi / 180.)
        self.interface.resetLocRequest.timestamp = int(self.time * 1000)