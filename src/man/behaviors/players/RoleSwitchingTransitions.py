def chaserIsOut(player):
    """
    There is a chaser spot ready to be filled.
    """
    if not player.roleSwitching:
        return False

    if not player.gameState == "gamePlaying":
        return False

    if player.role == 4 or player.role == 5:
        return False

    activePlayers = [False, False, False, False]
    opening = 0

    for mate in player.brain.teamMembers:
        if (mate.role == 2 and (mate.frameSinceActive < 30)):
            activePlayers[0] = True
        if (mate.role == 3 and (mate.frameSinceActive < 30)):
            activePlayers[1] = True
        if (mate.role == 4 and (mate.frameSinceActive < 30)):
            activePlayers[2] = True
        if (mate.role == 5 and (mate.frameSinceActive < 30)):
            activePlayers[3] = True

    if (activePlayers[2] and activePlayers[3]):
        return False # Both roles 4 and 5 are filled
    elif not activePlayers[3]:
        opening = 5
    elif not activePlayers[2]:
        opening = 4

    if player.role == 2:
        if activePlayers[1]:
            return False # Player in role 3 will take care of it
        else:
            player.openChaser = opening
            return True
    else:
        player.openChaser = opening
        return True

    return False

