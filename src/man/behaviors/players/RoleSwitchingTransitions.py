import RoleConstants as constants

def chaserIsOut(player):
    """
    There is a chaser spot ready to be filled.
    """
    print player.role
    checkForConsistency(player)

    if not player.roleSwitching:
        return False

    if not player.gameState == "gamePlaying":
        return False

    if constants.canRoleSwitchTo(player.role):
        return False

    openPosition = 0
    positions = [0, 0, 0, 0]
    for mate in player.brain.teamMembers:
        if mate.role <= 1:
            continue
        positions[mate.role - 2] += 1
        # print mate.playerNumber
        # print positions
        if mate.playerNumber == player.brain.playerNumber:
            continue
        if constants.canRoleSwitchTo(mate.role) and mate.frameSinceActive > 30:
            #print "position's open! -->", mate.role
            openPosition = mate.role

        if constants.willRoleSwitch(mate.role) \
                and mate.playerNumber > player.brain.playerNumber \
                and mate.frameSinceActive < 30:
            #print "mate's got it", mate.playerNumber, " ", mate.role

            return False # Active, higher numbered player takes precedence


    #print "------------------------------------------------"
    #print "openPosition is: ", openPosition
    if openPosition > 0 and positions[openPosition - 2] == 1:
        # for role in openPosition:
        #     if role[1] > 1:
        #         continue
        #     player.openChaser = role[0]
        player.openChaser = openPosition
        return True

    return False


def checkForConsistency(player):
    """
    Checks to see if anyone else has the same role as us. If they also have
    a lower playerNumber then we change. Otherwise we assume that they will
    fix the issue. Very similar in structure to determineRole in penalty states.
    """
    if not player.roleSwitching:
        return

    openSpaces = [True, True, True, True]
    conflict = False

    for mate in player.brain.teamMembers:
        openSpaces[mate.role - 2] = False
        if mate.role == player.role \
                and mate.playerNumber > player.brain.playerNumber \
                and mate.frameSinceActive < 30:
            conflict = True

    if not conflict:
        return # The expected outcome

    for i in range(3):
        if openSpaces[i] and roleConstants.canRoleSwitchTo(i+2):
            constants.setRoleConstants(player, i+2)
            return
        elif openSpaces[i]:
            position = i+2

    if position == 0:
        print "We have conflicting role AND there are no more open roles..."

    constants.setRoleConstants(player, position)
    return

