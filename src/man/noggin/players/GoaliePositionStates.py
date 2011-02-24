
import GoalieTransitions as helper
import GoalieConstants as constants

def goaliePosition(player):
    """
    Have the robot navigate to the position reported to it from playbook
    """
    brain = player.brain
    nav = brain.nav
    my = brain.my

    if player.firstFrame():
        player.isChasing = False
        nav.positionPlaybook()

    if brain.ball.dist >= constants.ACTIVE_LOC_THRESH:
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    #if helper.shouldPositionRight(player):
        #return player.goLater('goaliePositionRight')
    #elif helper.shouldPositionLeft(player):
        #return player.goLater('goaliePositionLeft')

    if helper.shouldPositionForSave(player):
        return player.goNow('goaliePositionForSave')


    heading = None
    ball = brain.ball

    return player.stay()

def goaliePositionRight(player):
#move to the right position.  need to add this position to the playbook

def goaliePositionLeft(player):
#move to the left position. need to add this position to the playbook

def goaliePositionForSave(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if helper.shouldSave(player):
        return player.goNow('goalieSave')

    #need to check if havent saved and need to move

    #Right now do not need to move side to side for saving
    #strafeDir = helper.strafeDirForSave(player)
    # if fabs(strafeDir) > 0:
        #player.setWalk(0, constants.STRAFE_SPEED * MyMath.sign(strafeDir), 0)
    # else:
        # player.stopWalking()

    return player.stay()
