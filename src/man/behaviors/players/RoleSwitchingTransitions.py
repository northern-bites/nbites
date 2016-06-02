import RoleConstants as constants

def chaserIsOut(player):
    """
    There is a chaser spot ready to be filled.
    """

    if player.dropIn:
        return False
    
    if not player.roleSwitching or player.brain.gameController.penalized:
        return False

    if not player.gameState == "gamePlaying":
        return False

    checkForConsistency(player)

    if constants.canRoleSwitchTo(player.intention):
        return False

    openPositions = ()
    positions = [False, False, False, False]
    for mate in player.brain.teamMembers:
        if mate.intention <= 1:
            continue
        if mate.frameSinceActive < 30:
            positions[mate.intention - 2] = True
        if mate.playerNumber == player.brain.playerNumber:
            continue
        if constants.canRoleSwitchTo(mate.intention) and mate.frameSinceActive > 30:
            openPositions += (mate.intention,)

        if constants.willRoleSwitch(mate.intention) \
                and mate.playerNumber > player.brain.playerNumber \
                and (mate.frameSinceActive < 30 or not mate.active):
            return False # Active, higher numbered player takes precedence


    for pos in openPositions:
        if not positions[pos - 2]:
            player.openChaser = pos
            print "Switching to role: ", pos
            return True

    return False


def checkForConsistency(player):
    """
    Checks to see if anyone else has the same role as us. If they also have
    a lower playerNumber then we change. Otherwise we assume that they will
    fix the issue. Very similar in structure to determineRole in penalty states.
    """
    if player.dropIn:
        return

    if not player.roleSwitching:
        return

    openSpaces = [True, True, True, True]
    conflict = False
    position = 0

    for mate in player.brain.teamMembers:
        if mate.playerNumber == player.brain.playerNumber:
            continue
        openSpaces[mate.intention - 2] = False
        if mate.intention == player.intention \
                and mate.playerNumber > player.brain.playerNumber \
                and mate.frameSinceActive < 30:
            conflict = True

    if not conflict:
        return # The expected outcome

    for i in range(3):
        if openSpaces[i] and constants.canRoleSwitchTo(i+2):
            constants.setRoleConstants(player, i+2)
            return
        elif openSpaces[i]:
            position = i+2

    if position == 0:
        print "We have conflicting role AND there are no more open roles..."

    constants.setRoleConstants(player, position)

    return



### DROP IN PLAYER ONLY ###