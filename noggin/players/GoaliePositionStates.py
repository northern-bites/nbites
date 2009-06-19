from ..playbook import PBConstants
from .. import NogginConstants
import GoalieTransitions as helper

CENTER_SAVE_THRESH = 15.
ORTHO_GOTO_THRESH = NogginConstants.CENTER_FIELD_X/2
STRAFE_ONLY = True
STRAFE_SPEED = 6
STRAFE_STEPS = 5
MAX_STEPS_OFF_CENTER = 50
BUFFER = 50

def goaliePosition(player):
    #consider using ball.x < fixed point- locDist could cause problems if
    #goalie is out of position. difference in accuracy?
    '''TODO-
    if shouldMoveToSave():
        player.goNow('goaliePositionForSave') '''
    if 0 < player.brain.ball.dist <= PBConstants.BALL_LOC_LIMIT:
        return player.goNow('goaliePositionBallClose')
    return player.goNow('goaliePositionBallFar')

def goaliePositionForSave(player):
    pass

def goaliePositionBallClose(player):

    if helper.shouldSave(player):
        player.shouldSaveCounter += 1
        if player.shouldSaveCounter >= 2:
            return player.goNow('goalieSave')
    else:
        player.shouldSaveCounter = 0

    if helper.shouldChase(player):
        player.shouldChaseCounter+=1
        if player.shouldChaseCounter >= 3:
            return player.goLater('goalieChase')
    else:
        player.shouldChaseCounter = 0

    brain = player.brain
    ball = brain.ball
    nav = brain.nav

    if ball.on:
        relY = ball.relY
    else:
        relY = 0

    brain.tracker.trackBall()

    if STRAFE_ONLY:
        if relY > CENTER_SAVE_THRESH and\
                player.stepsOffCenter < MAX_STEPS_OFF_CENTER:
            if player.setSteps(0, STRAFE_SPEED, 0, STRAFE_STEPS):
                player.stepsOffCenter += STRAFE_STEPS
        elif relY < -CENTER_SAVE_THRESH and\
                player.stepsOffCenter > -MAX_STEPS_OFF_CENTER:
            if player.setSteps(0, -STRAFE_SPEED, 0, STRAFE_STEPS):
                player.stepsOffCenter -= STRAFE_STEPS
            '''else:
                return player.goLater('goalieAtPosition')
        elif nav.isStopped():
            return player.goLater('goalieAtPosition')'''
    else:
        position = player.brain.playbook.position
        if nav.destX != position[0] or nav.destY != position[1] or\
                player.firstFrame():
            nav.orthoGoTo(position[0], position[1],
                          NogginConstants.OPP_GOAL_HEADING)
        # we're at the point, let's switch to another state
            '''if nav.isStopped() and player.counter > 0:
            return player.goLater('goalieAtPosition')'''

    #switch out if we lose the ball for multiple frames
    if (not (0 <= ball.dist <= PBConstants.BALL_LOC_LIMIT + BUFFER) or
        ball.framesOff > 15) and player.counter > 2:
        return player.goNow('goaliePositionBallFar')

    return player.stay()


def goaliePositionBallFar(player):

    if helper.shouldSave(player):
        player.shouldSaveCounter += 1
        if player.shouldSaveCounter >= 2:
            return player.goNow('goalieSave')
    else:
        player.shouldSaveCounter = 0

    if helper.shouldChase(player):
        player.shouldChaseCounter+=1
        if player.shouldChaseCounter >= 3:
            return player.goLater('goalieChase')
    else:
        player.shouldChaseCounter = 0

    brain = player.brain
    ball = brain.ball
    nav = brain.nav

    player.brain.tracker.activeLoc()

    if STRAFE_ONLY:
        if player.stepsOffCenter <= -STRAFE_STEPS:
            if player.setSteps(0, STRAFE_SPEED, 0, STRAFE_STEPS):
                player.stepsOffCenter += STRAFE_STEPS
        elif player.stepsOffCenter >= STRAFE_STEPS:
            if player.setSteps(0, -STRAFE_SPEED, 0, STRAFE_STEPS):
                player.stepsOffCenter -= STRAFE_STEPS
    else:
        position = player.brain.playbook.position
        useOrtho = True #(MyMath.dist(brain.my.x, brain.my.y, position[0],
                                   #position[1]) <= ORTHO_GOTO_THRESH)
        if nav.destX != position[0] or nav.destY != position[1] or\
                useOrtho!=nav.movingOrtho or player.firstFrame():
            if useOrtho:
                nav.orthoGoTo(position[0], position[1],
                              NogginConstants.OPP_GOAL_HEADING)
            else:
                nav.goTo(position[0], position[1],
                         NogginConstants.OPP_GOAL_HEADING)
    '''# we're at the point, let's switch to another state
    if nav.isStopped() and player.counter > 0:
        return player.goLater('goalieAtPosition')'''
    #don't switch out if we don't see the ball
    if 0 < ball.dist <= PBConstants.BALL_LOC_LIMIT - BUFFER and\
            player.counter > 2:
        return player.goLater('goaliePositionBallClose')
    return player.stay()

'''
def goalieAtPosition(player):
    """
    State for when we're at the position
    """
    if helper.shouldSave(player):
        player.shouldSaveCounter += 1
        if player.shouldSaveCounter >= 2:
            return player.goNow('goalieSave')
    else:
        player.shouldSaveCounter = 0

    if helper.shouldChase(player):
        player.shouldChaseCounter+=1
        if player.shouldChaseCounter >= 3:
            return player.goLater('goalieChase')
    else:
        player.shouldChaseCounter = 0

    brain = player.brain
    ball = brain.ball
    if 0 <= brain.ball.locDist <= PBConstants.BALL_LOC_LIMIT:
        brain.tracker.trackBall()
    else:
        brain.tracker.activeLoc()

    if ball.on:
        relY = brain.ball.relY
    else:
        relY = 0

    if STRAFE_ONLY:

        if (player.stepsOffCenter < MAX_STEPS_OFF_CENTER and
            relY > CENTER_SAVE_THRESH) or\
            (player.stepsOffCenter > -MAX_STEPS_OFF_CENTER and
             relY < -CENTER_SAVE_THRESH) and brain.nav.isStopped():
            return player.goLater('goaliePosition')
        elif (player.stepsOffCenter > STRAFE_STEPS or
              player.stepsOffCenter < -STRAFE_STEPS)
    elif brain.nav.notAtHeading(brain.nav.destH) or\
            not brain.nav.atDestinationCloser():
        return player.goLater('goaliePosition')

    return player.stay()

'''
