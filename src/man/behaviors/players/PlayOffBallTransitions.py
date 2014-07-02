import RoleConstants as role
import ClaimTransitions as claimTransitions
import SharedTransitions as shared

def ballInBox(player):
    """
    A transition which returns true if the ball is in the player's box
    """
    ball = player.brain.ball

    if ball.vis.frames_on > 0:
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

def ballInBufferedBox(player):
    """
    A transition which allows a stretching of a box so that the box isn't
    so ridged. Intended use is for in approachBall, ensuring that we don't loop
    between approachBall and positionAtHome if the ball is close to the edge of the box.
    """
    ball = player.brain.ball
    buf = role.boxBuffer

    if ball.vis.frames_on > 0:
        if role.isChaser(player.role):
            return True
        return (ball.x > player.box[0][0] - buf and ball.y > player.box[0][1] - buf and \
                ball.x < player.box[0][0] + player.box[1] + buf and \
                ball.y < player.box[0][1] + player.box[2] + buf)

def ballNotInBufferedBox(player):
    """
    Simple negation of ballInBufferedBox
    """
    return not ballInBufferedBox(player)

def tooFarFromHome(threshold):
    """
    Returns true if LOC thinks we're more than *distance* away from our home
    position
    """
    def transition(player):
        loc = player.brain.loc
        home = player.homePosition

        distance = ((loc.x - home.x)**2 + (loc.y - home.y)**2)**.5

        if distance > threshold:
            return True
        return False
    return transition

def shouldApproachBall(player):
    if ballNotInBox(player):
        return False

    if claimTransitions.shouldCedeClaim(player):
        return False

    return True

def shouldFindSharedBall(player):
    # Transition returns true if shared ball is on, and cannot see ball

    if shared.ballOffForNFrames(60) and player.brain.sharedBall.frames_on > 30:
        return True
    return False

def shouldStopLookingForSharedBall(player):
    if player.brain.sharedBall.frames_off > 60:
        return True
    return False

def shouldBeSupporter(player):
    return ballInBox(player) and claimTransitions.shouldCedeClaim(player)
