WAIT_FOR_CORNER_PASS_TIME = 9
PASS_TO_CORNER_TIME = 14
WALK_TO_CROSS_TIME = 12
SIDE_PASS_TIME = 8

BALL_ON_THRESH = 5
BALL_OFF_THRESH = 10

SIDE_PASS_OFFSET = 20.

def isSeeingBall(player):
    player.brain.ball.vis.frames_on > BALL_ON_THRESH

def ballIsLost(player):
    player.brain.ball.vis.frames_off > BALL_OFF_THRESH

def shouldPassToFieldCross(player):
    """
    when ball model is good enough, it should also check that the ball is no longer moving still
    """
    return (player.stateTime > WAIT_FOR_CORNER_PASS_TIME and self.isSeeingBall())

def ballNotPassedToCorner(player):
    """
    the ball probably was deflected or lost on the way to the corner or is was not even kicked there
    """
    return player.stateTime > PASS_TO_CORNER_TIME

def shouldStopWalkingToCross(player):
    return player.stateTime > WALK_TO_CROSS_TIME

def sidePassFinished(player):
    return player.brain.gameController.timeSincePlaying > SIDE_PASS_TIME