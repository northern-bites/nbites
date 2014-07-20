from . import RoleConstants as roleConstants
from math import hypot

WAIT_FOR_CORNER_PASS_TIME = 9
PASS_TO_CORNER_TIME = 17
WALK_TO_CROSS_TIME = 20
SIDE_PASS_TIME = 8
DID_NOT_RECIEVE_SIDE_PASS_TIME = 12

BALL_ON_THRESH = 5
BALL_OFF_THRESH = 10

SIDE_PASS_OFFSET = 20.

DIST_TO_POSITION_THRESH = 50

def shouldRunKickOffPlay(player):
    """
    makes sure the robots are in position to run a play
    """
    if not player.brain.ball.vis.on:
        return False

    if not roleConstants.twoAttackersOnField(player) or not player.brain.gameController.ownKickOff:
        # print "there were not two attackers on Field"
        return False

    if roleConstants.isFirstChaser(player.role):
        shouldRunKickOffPlay.distToPosition = hypot(player.brain.loc.x - roleConstants.ourKickoff.x,
                                                    player.brain.loc.y - roleConstants.ourKickoff.y)
        # print "my loc = ", player.brain.loc.x, ", ", player.brain.loc.y
        # print "should be ", roleConstants.ourKickoff.x, ", ", roleConstants.ourKickoff.y
    elif roleConstants.isSecondChaser(player.role) or roleConstants.isCherryPicker(player.role):
        shouldRunKickOffPlay.distToPosition = hypot(player.brain.loc.x - roleConstants.oddChaserKickoff.x,
                                                    player.brain.loc.y - roleConstants.oddChaserKickoff.y)
    else:
        shouldRunKickOffPlay.distToPosition = 0.

    if shouldRunKickOffPlay.distToPosition > DIST_TO_POSITION_THRESH:
        # print "I was ", shouldRunKickOffPlay.distToPosition, " away from position"
        return False

    for mate in player.brain.teamMembers:
        if roleConstants.isSecondChaser(mate.role) or roleConstants.isCherryPicker(mate.role):
            shouldRunKickOffPlay.distToPosition = hypot(mate.x - roleConstants.oddChaserKickoff.x,
                                                        mate.y - roleConstants.oddChaserKickoff.y)
            if not mate.ballOn:
                return False

        elif roleConstants.isFirstChaser(mate.role):
            shouldRunKickOffPlay.distToPosition = hypot(mate.x - roleConstants.ourKickoff.x,
                                                        mate.y - roleConstants.ourKickoff.y)
                if not mate.ballOn:
                    return False
            # print "mate loc = ", mate.x, ", ", mate.y
            # print "should be ", roleConstants.ourKickoff.x, ", ", roleConstants.ourKickoff.y
        else:
            shouldRunKickOffPlay.distToPosition = 0.

        if shouldRunKickOffPlay.distToPosition > DIST_TO_POSITION_THRESH:
            # print "Mate was ", shouldRunKickOffPlay.distToPosition, " away from position"
            return False

    return True

def isSeeingBall(player):
    return player.brain.ball.vis.frames_on > BALL_ON_THRESH

def ballIsLost(player):
    return player.brain.ball.vis.frames_off > BALL_OFF_THRESH

def shouldPassToFieldCross(player):
    """
    when ball model is good enough, this should also check that the ball is no longer moving still
    """
    return (player.stateTime > WAIT_FOR_CORNER_PASS_TIME and isSeeingBall(player))

def ballNotPassedToCorner(player):
    """
    the ball probably was deflected or lost on the way to the corner or is was not even kicked there
    """
    return player.stateTime > PASS_TO_CORNER_TIME

def shouldStopWalkingToCross(player):
    return player.stateTime > WALK_TO_CROSS_TIME

def sidePassFinished(player):
    return player.brain.gameController.timeSincePlaying > SIDE_PASS_TIME

def didNotRecieveSidePass(player):
    return player.stateTime > DID_NOT_RECIEVE_SIDE_PASS_TIME