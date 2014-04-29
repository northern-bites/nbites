import BoxPositionConstants as boxConstants
import ClaimTransitions as claimTransitions
#The transitions file for transitions exclusive to the Box-based positioning

def ballInBox(player):
    """
    A transition which returns true if the ball is in the player's box
    """
    ball = player.brain.ball

    if ball.x > player.box[0][0] and ball.y > player.box[0][1] and \
            ball.x < player.box[0][0] + player.box[1] and \
            ball.y < player.box[0][1] + player.box[2] and ball.vis.frames_on > 2:
        return True
    return False

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
    buf = boxConstants.boxBuffer

    if ball.x > player.box[0][0] - buf and ball.y > player.box[0][1] - buf and \
            ball.x < player.box[0][0] + player.box[1] + buf and \
            ball.y < player.box[0][1] + player.box[2] + buf and ball.vis.frames_on > 2:
        return True
    return False

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
