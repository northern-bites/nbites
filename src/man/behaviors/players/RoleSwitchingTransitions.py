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

    for mate in player.brain.teamMembers:
        if (mate.role == 4 or mate.role == 5) and not mate.active:
            player.openChaser = mate.role
            if player.openChaser - 2  == player.role:
                return True

    return False
