
"""
Game Controller States
"""
def gamePlaying(game):
    if game.firstFrame():
        game.brain.player.switchTo('gamePlaying')

    return game.stay()

def gamePenalized(game):
    if game.firstFrame():
        game.brain.player.switchTo('gamePenalized')

    return game.stay()

def gameInitial(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameInitial')

    return game.stay()

def gameSet(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameSet')

    return game.stay()

def gameReady(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameReady')

    return game.stay()

def gameFinished(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameFinished')

    return game.stay()
