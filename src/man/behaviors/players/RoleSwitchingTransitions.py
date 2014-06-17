import RoleConstants as constants

def chaserIsOut(player):
    """
    There is a chaser spot ready to be filled.
    """
    if not player.roleSwitching:
        return False

    if not player.gameState == "gamePlaying":
        return False

    if constants.isChaser(player.role):
        return False

    activePlayers = [False, False, False, False]
    opening = 0

    for mate in player.brain.teamMembers:
        if (constants.isLeftDefender(mate.role) and (mate.frameSinceActive < 30)):
            activePlayers[0] = True
        if (constants.isRightDefender(mate.role) and (mate.frameSinceActive < 30)):
            activePlayers[1] = True
        if (constants.isFirstChaser(mate.role) and (mate.frameSinceActive < 30)):
            activePlayers[2] = True
        if (constants.isSecondChaser(mate.role) and (mate.frameSinceActive < 30)):
            activePlayers[3] = True

    if (activePlayers[2] and activePlayers[3]):
        return False # Both chaser roles are filled
    elif not activePlayers[3]:
        opening = 5
    elif not activePlayers[2]:
        opening = 4

    if constants.isLeftDefender(player.role):
        if activePlayers[1]:
            return False # Right defender will take care of it
        else:
            player.openChaser = opening
            return True # We're the one who needs to deal with it!
    else:
        player.openChaser = opening
        return True # We're right defender, will switch

    return False

