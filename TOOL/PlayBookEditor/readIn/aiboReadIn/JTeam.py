"""
File to hold supporter classes for reading in playbook data
"""
import Constants
from math import hypot
class JTeam:
    """
    All the necessary parts from GoTeam.py
    """
    # Value is tested in subRoles
    kickoffformation = 0

    def __init__(self):
        self.brain = JBrain()
        self.moveBall = True
        self.kickoffFormation = 0

    def moveBallRepulsor(self):
        self.moveBall = True

    def removeBallRepulsor(self):
        self.moveBall = False
    def getPointBetweenBallAndGoal(self,dist_from_goal):
	'''returns defensive position between ball (x,y) and goal (x,y)
	at <dist_from_ball> centimeters away from ball'''

        leftPostToBall = hypot(Constants.LANDMARK_MY_GOAL_LEFT_POST_X -
                           self.brain.ball.x,
                           Constants.LANDMARK_MY_GOAL_LEFT_POST_Y -
                           self.brain.ball.y)

        rightPostToBall = hypot(Constants.LANDMARK_MY_GOAL_RIGHT_POST_X -
                            self.brain.ball.x,
                            Constants.LANDMARK_MY_GOAL_RIGHT_POST_Y -
                            self.brain.ball.y)

        goalLineIntersectionX = Constants.LANDMARK_MY_GOAL_LEFT_POST_X +\
            (leftPostToBall*Constants.GOAL_WIDTH)/(leftPostToBall+rightPostToBall)

        ballToInterceptDist = hypot(self.brain.ball.y -
                                    Constants.LANDMARK_MY_GOAL_LEFT_POST_Y,
                                    self.brain.ball.x - goalLineIntersectionX)

        pos_x = ((dist_from_goal / ballToInterceptDist)*
                 (self.brain.ball.x -goalLineIntersectionX) +
                 goalLineIntersectionX)

        pos_y = ((dist_from_goal / ballToInterceptDist)*
                 (self.brain.ball.y -
                  Constants.LANDMARK_MY_GOAL_LEFT_POST_Y) +
                 Constants.LANDMARK_MY_GOAL_LEFT_POST_Y)

	return pos_x,pos_y

class JBrain:
    """
    Simple brain methods we need to reference
    """
    def __init__(self):
        self.game = JGame()
        self.ball = JBall()

    def clip(self, val, min, max):
        if val < min:
            return min
        elif val > max:
            return max
        else:
            return val
class JBall:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

class JGame:
    def __init__(self):
        self.myTeam = JMyTeam()

class JMyTeam:
    def __init__(self):
        self.kickOff = True

class SubRole:
    def __init__(self):
        name = None
        minX = 0
        maxX = 0
        xOffset = 0

        minY = 0
        maxY = 0
        yOffset = 0
        ballRepulsor = False
