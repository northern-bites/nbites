
###
# Reimplementation of Game Controller States for pBrunswick
###

def gameInitial(player):
    return player.goNow('saveFrames')

def saveFrames(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
    if player.counter % 15 == 0:
        player.brain.sensors.saveFrame()
    if player.counter > 15 * 150:
        return player.goNow('doneState')

    return player.stay()

def doneState(player):
    player.brain.tracker.stopHeadMoves()
    return player.stay()
