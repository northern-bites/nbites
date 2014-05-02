def chaserIsOut(player):
    """
    There is no chaser currently in the game and we are ready to fill the spot.
    """
    if not player.brain.teamMembers[player.brain.playerNumber-1].active:
        return False

    for mate in player.brain.teamMembers:
        if mate.role == 4 and mate.active:
            return False
    return True
