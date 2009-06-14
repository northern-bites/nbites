from ..playbook import PBConstants
from .. import NogginConstants
import GoalieTransitions as helper

CENTER_SAVE_THRESH = 15.
ORTHO_GOTO_THRESH = NogginConstants.CENTER_FIELD_X/2
STRAFE_ONLY = True
STRAFE_SPEED = 6
STRAFE_STEPS = 5

def goaliePosition(player):

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

    if 0 <= ball.locDist <= PBConstants.BALL_LOC_LIMIT*(3./4.):
        brain.tracker.trackBall()
    else:
        brain.tracker.activeLoc()
    if ball.on:
        relY = ball.relY
    else:
        relY = 0

    if STRAFE_ONLY:
        if relY > CENTER_SAVE_THRESH and nav.isStopped():
            nav.setSteps(0, STRAFE_SPEED, 0, STRAFE_STEPS)
            nav.switchTo('stepping')
        elif relY < -CENTER_SAVE_THRESH and nav.isStopped():
            nav.setSteps(0, -STRAFE_SPEED, 0, STRAFE_STEPS)
            nav.switchTo('stepping')
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
    # we're at the point, let's switch to another state
    if nav.isStopped() and player.counter > 0:
        return player.goLater('goalieAtPosition')

    return player.stay()

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
        if brain.nav.isStopped() and abs(relY) > CENTER_SAVE_THRESH:
            return player.goLater('goaliePosition')
    elif brain.nav.notAtHeading(brain.nav.destH) or\
            not brain.nav.atDestinationCloser():
        return player.goLater('goaliePosition')

    return player.stay()

