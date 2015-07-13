import RoleConstants as constants

def chaserIsOut(player):
    """
    There is a chaser spot ready to be filled.
    """
    if not player.roleSwitching or player.brain.gameController.penalized:
        return False

    if not player.gameState == "gamePlaying":
        return False

    checkForConsistency(player)

    if constants.canRoleSwitchTo(player.role):
        return False

    openPositions = ()
    positions = [False, False, False, False]
    for mate in player.brain.teamMembers:
        if mate.role <= 1:
            continue
        if mate.frameSinceActive < 30:
            positions[mate.role - 2] = True
        if mate.playerNumber == player.brain.playerNumber:
            continue
        if constants.canRoleSwitchTo(mate.role) and mate.frameSinceActive > 30:
            openPositions += (mate.role,)

        if constants.willRoleSwitch(mate.role) \
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
    if not player.roleSwitching:
        return

    openSpaces = [True, True, True, True]
    conflict = False
    position = 0

    for mate in player.brain.teamMembers:
        if mate.playerNumber == player.brain.playerNumber:
            continue
        openSpaces[mate.role - 2] = False
        if mate.role == player.role \
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


def determineOpenRoles(player):
    """
    Used for role switching in drop in games. Similar to structures of determineRole
    in PenaltyStates and checkForConsistency above.
    """

    if player.brain.activeTeamMates() > 4:
        print "Sufficient number of teammates."
        return

    if not player.roleSwitching:
        return

    if not player.gameState == "gamePlaying":
        return

    openSpaces = [True, True, True, True] 
    openSpaces[player.role - 2] = False
    conflict = -1

    print "I have", player.brain.activeTeamMates(), "active teammates."
    #number of defenders and number of offense players
    for mate in player.brain.teamMembers:
        print "Player", mate.playerNumber, "has role", player.role
        if constants.isGoalie(mate.role):
            continue
        if constants.isDefender(mate.role) and mate.frameSinceActive > 30:
            openSpaces[mate.role - 2] = False
            print "Player", mate.playerNumber, "is a defender."
            continue
        if constants.isChaser(mate.role) and mate.frameSinceActive > 30:
            openSpaces[mate.role -2] = False
            print "Player", mate.playerNumber, "is a chaser/striker."
            continue
        if player.role == mate.role:
            if mate.playerNumber == player.brain.playerNumber:
                continue 
            print "There is role overlap in role", mate.role, "with player number", mate.playerNumber
            conflict = player.role

    #if robots overlap on a position that is not the goalie
    #switch roles, prioritizing higher positions
    if conflict > 1:
        print "Conflict value is ", conflict
        for i in range(3, -1, -1):
            if openSpaces[i] == True:
                switchToRole(player, i+2)
                return

    if offenseOpen and not defenseOpen(openSpaces):
        for i in range(3, -1, -1):
            if openSpaces[i] == True:
                switchToRole(player, i+2)
                return
        
    # elif defenseOpen(openSpaces) and not offenseOpen(openSpaces):
    #     print "Defense was open, switching to defense."
    #     for i in range(3):
    #         if openSpaces[i] == True:
    #             switchToRole(player, i+2)
    #             return

    return

def switchToRole(player,role):
    print "Switching to role", role
    player.role = role
    constants.setRoleConstants(player, role, -1)
    return

def offenseOpen(openSpaces):
    print "Testing if both chaser/striker are on the field."
    if openSpaces[2] == openSpaces[3] == True:
        return True

def defenseOpen(openSpaces):
    print "Testing if both defenders are on the field."
    if openSpaces[0] == openSpaces[1] == True:
        return True
