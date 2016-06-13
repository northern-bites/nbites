import RoleConstants as role
import ClaimTransitions as claimTransitions
import SharedTransitions as shared
import ChaseBallConstants as chaseConstants
import noggin_constants as nogginC
from objects import RobotLocation
from SupporterConstants import calculateHomePosition
from math import fabs

def ballInBox(player):
    """
    A transition which returns true if the ball is in the player's box
    """
    ball = player.brain.ball

    if ball.vis.frames_on > chaseConstants.BALL_ON_THRESH:
        return (ball.x > player.box[0][0] and ball.y > player.box[0][1] and
                ball.x < player.box[0][0] + player.box[1] and
                ball.y < player.box[0][1] + player.box[2])

def ballNotInBox(player):
    """
    A transition which returns true if the ball isn't in the player's box
    """
    return not ballInBox(player)

def ballNotInBufferedBox(player):
    """
    A transition which allows a stretching of a box so that the box isn't
    so ridged. Intended use is for in approachBall, ensuring that we don't loop
    between approachBall and positionAtHome if the ball is close to the edge of the box.
    """
    ball = player.brain.ball
    buf = role.boxBuffer
    inBox = (ball.x > player.box[0][0] - buf and ball.y > player.box[0][1] - buf and \
            ball.x < player.box[0][0] + player.box[1] + buf and \
            ball.y < player.box[0][1] + player.box[2] + buf)

    return (ball.vis.frames_off > chaseConstants.BALL_OFF_THRESH or 
            (not inBox and not role.isFirstChaser(player.role)))

def tooFarFromHome(player, distThreshold, angleThreshold):
    """
    Returns true if LOC thinks we're more than *distance* away from our home
    position
    """
    if role.isDefender(player.role):
        home = calculateHomePosition(player)
    else:
        home = player.homePosition

    distanceTo = ((player.brain.loc.x - home.x)**2 + (player.brain.loc.y - home.y)**2)**.5
    angleTo = fabs(player.brain.loc.h - home.h)

    return distanceTo > distThreshold or angleTo > angleThreshold

def shouldSpinSearchFromWatching(player):
    shouldExtendTimer = player.commMode == 2 and role.isDefender(player.role)
    spinTimer = 25 if shouldExtendTimer else 12
    return (player.stateTime > spinTimer and
            player.brain.ball.vis.frames_off > 30)
  
def stopSpinning(player):
    return player.brain.ball.vis.frames_on > 0
  
def shouldApproachBall(player):
    if ballNotInBox(player):
        player.claimedBall = False
        return False

    if claimTransitions.shouldCedeClaim(player):
        return False

    return True

def shouldFindSharedBall(player):
    return (role.isFirstChaser(player.role) and
            player.brain.ball.vis.frames_off > 10 and
            player.brain.sharedBall.ball_on and
            player.brain.sharedBall.reliability >= 1)

def noBallFoundAtSharedBall(player):
    return (player.sharedBallCloseCount >= 60 and 
            player.brain.sharedBall.ball_on)

def shouldFindFlippedSharedBall(player):
    return noBallFoundAtSharedBall(player) and player.brain.sharedBall.reliability >= 2

def shouldStopLookingForSharedBall(player):
    return player.sharedBallOffCount >= 105

def shouldStopLookingForFlippedSharedBall(player):
    return noBallFoundAtSharedBall(player) or shouldStopLookingForSharedBall(player)

def shouldBeSupporter(player):
    if not player.brain.motion.calibrated:
        player.claimedBall = False
        return False
        
    return (ballInBox(player) and
            claimTransitions.shouldCedeClaim(player))

def shouldNotBeSupporter(player):
    if role.isChaser(player.role):
        return shared.ballOffForNFrames(120)(player)
    return not shouldBeSupporter(player)
