# Test class for landmark tracking localization system

import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    player.gainsOn()
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn
        player.standup()
    return player.stay()

def gameSet(player):
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.tracker.readyLoc()
    #if player.counter > 30:
    #    player.brain.tracker.nudge()
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.brain.tracker.nudge()
    return player.goLater('gamePlaying')
