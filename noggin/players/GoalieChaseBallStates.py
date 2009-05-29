import BrunswickGoalieTransitions as helper

def goalieChase(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()
    if helper.shouldSave(player):
        return player.goNow('goalieSave')
    if helper.shouldStopChase(player):
        return player.goLater('goaliePosition')
    return player.stay()