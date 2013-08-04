#The transitions file for transitions exclusive to the Box-based positioning

def ballInBox(player):
    """
    A transition which returns true if the ball is in the player's box
    """
    if not player.usingBoxPositions:
        return True
    ball = player.brain.ball
    
    if ball.x > player.box[0][0] and ball.y > player.box[0][1] and \
            ball.x < player.box[0][0] + player.box[1] and \
            ball.y < player.box[0][1] + player.box[2] and ball.vis.frames_on > 2:
        return True
    return False


def ballNotInBox(player):
    """
    A transition which returns true if the ball is in the player's box
    """
    if not player.usingBoxPositions:
        return False

    ball = player.brain.ball
    
    if ball.x > player.box[0][0] and ball.y > player.box[0][1] and \
            ball.x < player.box[0][0] + player.box[1] and \
            ball.y < player.box[0][1] + player.box[2] and ball.vis.frames_on > 2:
        return False
    return True
