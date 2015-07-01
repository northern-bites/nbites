import RoleConstants as role
import ClaimTransitions as claimTransitions
import SharedTransitions as shared
import ChaseBallConstants as chaseConstants
import noggin_constants as nogginC
from objects import RobotLocation
from SupporterConstants import findChaserHome, findDefenderHome

def ballInBox(player):
    """
    A transition which returns true if the ball is in the player's box
    """
    ball = player.brain.ball

    if ball.vis.frames_on > chaseConstants.BALL_ON_THRESH:
        if role.isChaser(player.role):
            return True
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
            (not inBox and not role.isChaser(player.role)))

def tooFarFromHome(threshold, player):
    """
    Returns true if LOC thinks we're more than *distance* away from our home
    position
    """
    if player.brain.ball.vis.on:
        ball = player.brain.ball
    elif player.brain.sharedBall.ball_on:
        ball = player.brain.sharedBall
    else:
        ball = None
        home = player.homePosition

    if ball != None:
        if role.isLeftDefender(player.role):
            home = findDefenderHome(True, ball, player.homePosition.h)
        elif role.isRightDefender(player.role):
            home = findDefenderHome(False, ball, player.homePosition.h)
        elif role.isSecondChaser(player.role):
            home = findChaserHome(ball, player.homePosition.h)
        else:
            home = player.homePosition

    distance = ((player.brain.loc.x - home.x)**2 + (player.brain.loc.y - home.y)**2)**.5

    return distance > threshold
  
def shouldApproachBall(player):
    if ballNotInBox(player):
        player.claimedBall = False
        return False

    if claimTransitions.shouldCedeClaim(player):
        return False

    return True

def shouldFindSharedBall(player):
    return (player.brain.sharedBall.ball_on and
            player.brain.sharedBall.reliability >= 1)

def shouldFindFlippedSharedBall(player):
    return player.sharedBallCloseCount >= 60

def shouldStopLookingForSharedBall(player):
    return player.sharedBallOffCount >= 105

def shouldStopLookingForFlippedSharedBall(player):
    return shouldFindFlippedSharedBall(player) or shouldStopLookingForSharedBall(player)

def shouldBeSupporter(player):
    if not player.brain.motion.calibrated:
        player.claimedBall = False
        return False
        
    return (ballInBox(player) and
            claimTransitions.shouldCedeClaim(player))
