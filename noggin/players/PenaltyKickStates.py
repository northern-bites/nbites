
def penaltyKick(player):
    player.penaltyKicking = True
    return player.goNow('approachBallWithLoc')

