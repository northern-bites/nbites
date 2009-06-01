import GoalieTransitions as helper

def goalieChase(player):
    if player.firstFrame():
        player.stopWalking()
    if not player.trackingBall:
        player.brain.tracker.trackBall()
        player.trackingBall = True
    if helper.shouldSave(player):
        return player.goNow('goalieSave')
    if not helper.shouldChase(player):
        return player.goLater('goaliePosition')
    return player.stay()
