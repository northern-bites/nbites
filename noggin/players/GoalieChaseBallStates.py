import GoalieTransitions as helper

def goalieChase(player):
    if player.firstFrame():
        player.shouldChaseCounter = 0
        player.stopWalking()
        player.brain.tracker.trackBall()

    if helper.shouldSave(player):
        player.shouldSaveCounter += 1
        if player.shouldSaveCounter >= 2:
            return player.goNow('goalieSave')
    else:
        player.shouldSaveCounter = 0

    if not helper.shouldChase(player):
        player.shouldChaseCounter+=1
        if player.shouldChaseCounter >= 3:
            player.shouldChaseCounter = 0
            return player.goLater('goaliePosition')
    else:
        player.shouldChaseCounter += 1

    return player.stay()
