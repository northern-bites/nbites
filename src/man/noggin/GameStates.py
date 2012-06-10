"""
Game Controller States
"""
def gameInitial(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameInitial')
    return game.stay()

def gameReady(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameReady')
    return game.stay()

def gameSet(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameSet')
    return game.stay()

def gamePlaying(game):
    if game.firstFrame():
        game.brain.player.switchTo('gamePlaying')
    return game.stay()

def gamePenalized(game):
    if game.firstFrame():
        game.brain.player.switchTo('gamePenalized')
    return game.stay()

def gameFinished(game):
    if game.firstFrame():
        game.brain.player.switchTo('gameFinished')
    return game.stay()

def penaltyShotsGameInitial(game):
    if game.firstFrame():
        game.brain.player.switchTo('penaltyShotsGameInitial')
    return game.stay()

def penaltyShotsGameReady(game):
    if game.firstFrame():
        game.brain.player.switchTo('penaltyShotsGameReady')
    return game.stay()

def penaltyShotsGameSet(game):
    if game.firstFrame():
        game.brain.player.switchTo('penaltyShotsGameSet')
    return game.stay()

def penaltyShotsGamePlaying(game):
    if game.firstFrame():
        game.brain.player.switchTo('penaltyShotsGamePlaying')
    return game.stay()

def penaltyShotsGamePenalized(game):
    if game.firstFrame():
        game.brain.player.switchTo('penaltyShotsGamePenalized')
    return game.stay()

def penaltyShotsGameFinished(game):
    if game.firstFrame():
        game.brain.player.switchTo('penaltyShotsGameFinished')
    return game.stay()
