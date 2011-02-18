import man.motion.SweetMoves as SweetMoves
import GoalieTransitions as helper


CENTER_SAVE_THRESH = 15

def goalieSave(player):
    brain = player.brain
    if player.firstFrame():
        player.saving = True
        player.isChasing = False
        brain.motion.stopHeadMoves()
        player.stopWalking()
        brain.tracker.trackBall()
    if not brain.nav.isStopped():
        return player.stay()
    ball = brain.ball

    print "Should be saving center"
    return player.goNow('saveCenter')

def saveRight(player):
    if player.firstFrame():
        player.brain.guard.disable()
        player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.GOALIE_DIVE_RIGHT):
        return player.goLater('holdRightSave')
    return player.stay()

def saveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_DIVE_LEFT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.GOALIE_DIVE_LEFT):
        return player.goLater('holdLeftSave')
    return player.stay()

def saveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT)
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT):
        return player.goLater('holdCenterSave')
    return player.stay()

def holdRightSave(player):
    if player.brain.nav.isStopped():
        player.brain.guard.enable()
        player.saving = False
        player.brain.tracker.trackBall()
        return player.goLater('postSave')

    return player.stay()


def holdLeftSave(player):
    #when dive works want wait time for now just go
    if player.brain.nav.isStopped():
        player.brain.guard.enable()
        player.saving = False
        player.brain.tracker.trackBall()
        return player.goLater('postSave')

    return player.stay()


def holdCenterSave(player):
    if player.brain.nav.isStopped():# what does this do?
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
        player.saving = False
        player.brain.tracker.trackBall()
        return player.goLater('postSave')
    
    return player.stay()

def postSave(player):
    if firstFrame():
        player.brain.guard.enable()
    return player.stay()
