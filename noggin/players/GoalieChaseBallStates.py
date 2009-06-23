import GoalieTransitions as helper

def goalieChase(player):
    if player.firstFrame():
        player.shouldChaseCounter = 0
        player.brain.tracker.trackBall()

    if helper.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif helper.shouldApproachBallWithLoc(player):
        return player.goNow('approachBallWithLoc')
    elif helper.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif helper.shouldKick(player):
        return player.goNow('waitBeforeKick')
    elif helper.shouldTurnToBall_ApproachBall(player):
        return player.goNow('turnToBall')
    elif helper.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')
    else:
        return player.goNow('scanFindBall')

def goalieScanFindBall(player):

    return player.stay()
