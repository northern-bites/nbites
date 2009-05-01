from .. import NogginConstants as Constants
from . import ChaseBallConstants as ChaseConstants
import ChaseBallTransitions as chaseTransitions

def positionLocalize(player):

    return player.stay()

def playbookPosition(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.activeLoc()
        player.printf("I am going to " + str(player.brain.playbook.position))

    position = player.brain.playbook.position

    if player.brain.nav.destX != position[0] or \
            player.brain.nav.destY != position[1]:
        player.brain.nav.goTo(position[0], position[1], Constants.OPP_GOAL_HEADING)
        #player.printf("position = "+str(position[0])+" , "+str(position[1]) )

    # we're at the point, let's switch to another state
#     if player.brain.nav.isStopped() and player.counter > 0:
#         pass
    return player.stay()

def positionOnBall(player):
    # for now we want a way to not rely on localization for chasing
    if not ChaseConstants.USE_LOC_CHASE:
        if chaseTransitions.shouldApproachBall(player):
            return player.goNow('approachBall')
        elif chaseTransitions.shouldTurnToBall_ApproachBall(player):
            return player.goNow('turnToBallFar')
        elif chaseTransitions.shouldSpinFindBall(player):
            return player.goNow('spinFindBall')
        elif chaseTransitions.shouldTurnToBallClose(player):
            return player.goNow('turnToBallClose')
        else :
            return player.goNow('scanFindBall')

    nextX, nextY,nextH = player.getBehindBallPosition()
    #player.printf("position = "+str(nextX)+" , "+str(nextY) )
    if player.brain.nav.destX != nextX or \
            player.brain.nav.destY != nextY:
        player.brain.nav.goTo(nextX,nextY,nextH)

    if player.brain.nav.isStopped():
        player.goLater('chase')

    return player.stay()
