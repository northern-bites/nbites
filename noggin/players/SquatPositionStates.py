import man.motion.SweetMoves as SweetMoves
import GoalieConstants as constants
import GoalieTransitions as helper
from .. import NogginConstants

def squat(player):
    if player.firstFrame() and not player.squatting:
        player.executeMove(SweetMoves.INITIAL_POS)
        player.squatting = True
        player.executeMove(SweetMoves.GOALIE_SQUAT)
    if not player.motion.isBodyActive():
        return player.goLater('squatted')
    return player.stay()

def squatted(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
    #if my current position == getProperPosition
    #or wrap in moveSquattedPosition
    return player.stay()

def squatPosition(player):

    brain = player.brain
    position = brain.playbook.position
    nav = brain.nav
    my = brain.my

    if player.firstFrame():
        player.changeOmniGoToCounter = 0
        player.isChasing = False

    if brain.ball.x >= constants.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    useOmni = helper.useOmni(player)
    changedOmni = False

    ball = brain.ball
    bearing = None
    if ball.on:
        bearing = ball.bearing
    elif ball.framesOff < 3:
        bearing = ball.locBearing
    else:
        bearing = NogginConstants.OPP_GOAL_HEADING

    if (not nav.atDestinationGoalie() or
        not nav.atHeading()):
        if not useOmni:
            nav.goTo((position[0], position[1], my.h + bearing))
        else:
            nav.omniGoTo((position[0], position[1], my.h + bearing))
    else:
        return player.goLater("squat")

    return player.stay()

def chasePrepare(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.isChasing = False

    if player.squatting:
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
        player.squatting = False
        return player.stay()

    if not player.motion.isBodyActive():
        player.isChasing = True
        return player.goLater('chase')
    return player.stay()
