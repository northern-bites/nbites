# pMBA.py -- Nao Soccer Player, written in Suzhou, China
#
#
#@author Joho Strom
#@author Jack Morrison
#
#
#
#
import util.FSA as FSA
import math
import SuzhouStates
import util.MyMath as MyMath # For getting shooting pos
import SuzhouConstants as PlayerConstants

class SoccerPlayer(FSA.FSA):
    def __init__(self,brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain
        self.addStates(SuzhouStates)
        self.currentState = 'initial'
        self.setName('Player pSuzhou')
        self.setPrintStateChanges(True)
        self.setPrintFunction(self.brain.out.printf)

    
    ################  HELPER METHODS #################
    
    
    def getShootingPos(self,dist):
        ''' Returns where to position from ball location.
        Puts the robot in line with ball and goal center.'''
        destX,destY,destH = 0,0,0
        m = ((OPP_GOALBOX_TOP_Y - self.brain.ball.y) /
             (NogginConstants.MIDFIELD_X - self.brain.ball.x))
             
        destX = self.brain.ball.x - math.sqrt(dist**2 / (1 + m**2))

        destY = m * (destX - self.brain.ball.x) + self.brain.ball.y        
        if m == 0:
            destH = 0
        else:
            destH = MyMath.getRelativeHeading(destX, destY,
                                              self.brain.ball.x,
                                              self.brain.ball.y)
        return destX, destY, destH


    def shouldRefind(self):
        ''' Should we spin and look for the ball again. '''
        return (self.brain.ball.framesOff > 
                PlayerConstants.REFIND_BALL_FRAMES_OFF)

    def shouldApproachBall(self):
        ''' Should we move to kicking position '''
        errors = self.brain.nav.getErrors()
        distError = (errors[0]**2 + errors[1]**2) ** .5
        headingError = errors[2]
        return (distError < PlayerConstants.DIST_ERROR_THRESH and
                headingError < PlayerConstants.HEADING_ERROR_THRESH)

    def shouldDefend(self):
        ''' Eventually: return true when we are called off
        the ball, possibly due to the close proximity of another robot.'''
        return False

    def shouldChase(self):
        return self.brain.ball.on
