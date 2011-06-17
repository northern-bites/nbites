from .. import NogginConstants
from . import PBConstants
from . import SubRoles
from . import RoleTransitions as RoleTran

#######################################
# In each role, we check to see if we should switch into a particular subRole
# This prevents hysteresis. We also set the subRoleSwitchTime to a special
# value (i.e. -1) if we are in a new role because we want to reset our counter for that.

def rGoalie(team, workingPlay):
    '''The Goalie'''
    ''' We don't use shouldSwitchSubRole because of need to switch at
    different points for different actions with the goalies. Use
    counters instead to keep track of when to change.  Counters are done
    through RoleTransitions. '''
    if not workingPlay.isRole(PBConstants.GOALIE):
        workingPlay.setRole(PBConstants.GOALIE)
    if (team.brain.gameController.currentState == 'gameReady' or
        team.brain.gameController.currentState =='gameSet'):
        # Special case for ready goalie.
        SubRoles.pReadyGoalie(team, workingPlay)

    elif workingPlay.isSubRole(PBConstants.READY_GOALIE):
        SubRoles.pGoalieNormal(team, workingPlay)

    elif workingPlay.isSubRole(PBConstants.GOALIE_PENALTY_SAVER):
        SubRoles.pGoaliePenaltySaver(team, workingPlay)

    elif workingPlay.isSubRole(PBConstants.GOALIE_CHASER):
        if RoleTran.shouldStopChase(team):
            SubRoles.pGoalieNormal(team, workingPlay)

        elif RoleTran.shouldPosition(team):
            SubRoles.pGoalieNormal(team, workingPlay)

        else:
            SubRoles.pGoalieChaser(team, workingPlay)

    elif (workingPlay.isSubRole(PBConstants.GOALIE_NORMAL) or
          workingPlay.isSubRole(PBConstants.GOALIE_RIGHT) or
          workingPlay.isSubRole(PBConstants.GOALIE_LEFT)):
        if RoleTran.shouldChase(team):
            SubRoles.pGoalieChaser(team, workingPlay)

        # elif RoleTran.shouldPositionRight(team):
        #     SubRoles.pGoaliePosRight(team, workingPlay)

        # elif RoleTran.shouldPositionLeft(team):
        #     SubRoles.pGoaliePosLeft(team, workingPlay)

        elif RoleTran.shouldPositionCenter(team):
            SubRoles.pGoalieNormal(team, workingPlay)

        elif workingPlay.isSubRole(PBConstants.GOALIE_NORMAL):
            SubRoles.pGoalieNormal(team, workingPlay)

        # elif workingPlay.isSubRole(PBConstants.GOALIE_RIGHT):
        #     SubRoles.pGoaliePosRight(team, workingPlay)

        # elif workingPlay.isSubRole(PBConstants.GOALIE_LEFT):

        #     SubRoles.pGoaliePosLeft(team, workingPlay)


def rChaser(team, workingPlay):
    '''sets current position for chaser since positioning is done by player'''
    if not workingPlay.isRole(PBConstants.CHASER):
        team.subRoleSwitchTime = 0
    workingPlay.setRole(PBConstants.CHASER)
    # Note: Don't use shouldSwitchSubRole here. Go for the ball immediately.
    workingPlay.setSubRole(PBConstants.CHASE_NORMAL)
    pos = (team.brain.my.x,team.brain.my.y, team.brain.ball.heading)
    workingPlay.setPosition(pos)

def rDefender(team, workingPlay):
    '''gets positioning for defender'''
    if not workingPlay.isRole(PBConstants.DEFENDER):
        team.subRoleSwitchTime = -1
    workingPlay.setRole(PBConstants.DEFENDER)
    if team.brain.ball.x < PBConstants.SWEEPER_X_THRESH:
        subRoleOnDeck = PBConstants.SWEEPER
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pSweeper(team, workingPlay)
    elif team.brain.ball.x < PBConstants.CENTER_BACK_X_THRESH:
        subRoleOnDeck = PBConstants.CENTER_BACK
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pCenterBack(team, workingPlay)
    else:
        subRoleOnDeck = PBConstants.STOPPER
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pStopper(team, workingPlay)

def rDefenderDubD(team, workingPlay):
    '''handles condition when we have two defenders'''
    if not workingPlay.isRole(PBConstants.DEFENDER_DUB_D):
        team.subRoleSwitchTime = -1
    workingPlay.setRole(PBConstants.DEFENDER_DUB_D)
    other_mates = None # put in scope
    myNumber = team.me.playerNumber
    if team.numActiveFieldPlayers == 2:
        other_mates = team.getOtherActiveFieldPlayers([myNumber])
    elif team.numActiveFieldPlayers == 3:
        # More forward robot is not a defender
        forwardNumber = team.getForward(team.activeFieldPlayers).playerNumber
        other_mates = team.getOtherActiveFieldPlayers([forwardNumber, myNumber])

    rightPos = PBConstants.RIGHT_DEEP_BACK_POS
    leftPos = PBConstants.LEFT_DEEP_BACK_POS
    # Figure out who should go to which position
    pos = team.getLeastWeightPosition((rightPos,leftPos), other_mates)
    if pos == rightPos:
        subRoleOnDeck = PBConstants.RIGHT_DEEP_BACK
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pRightDeepBack(team, workingPlay)
    else: #if pos == leftPos
        subRoleOnDeck = PBConstants.LEFT_DEEP_BACK
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pLeftDeepBack(team, workingPlay)

def rOffender(team, workingPlay):
    '''The offensive attacker!'''
    if not workingPlay.isRole(PBConstants.OFFENDER):
        team.subRoleSwitchTime = -1
    workingPlay.setRole(PBConstants.OFFENDER)

    ballX = team.brain.ball.x
    ballY = team.brain.ball.y
    if ballX > PBConstants.PICKER_X_THRESH:
        subRoleOnDeck = PBConstants.PICKER
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pPicker(team, workingPlay)
    elif ballX > PBConstants.WING_X_THRESH:
        if ballY < NogginConstants.CENTER_FIELD_Y:
            subRoleOnDeck = PBConstants.LEFT_WING
            if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
                SubRoles.pLeftWing(team, workingPlay)
        else:
            subRoleOnDeck = PBConstants.RIGHT_WING
            if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
                SubRoles.pRightWing(team, workingPlay)
    elif ballX > PBConstants.STRIKER_X_THRESH:
        subRoleOnDeck = PBConstants.STRIKER
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pStriker(team, workingPlay)
    else:
        subRoleOnDeck = PBConstants.FORWARD
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pForward(team, workingPlay)

def rMiddie(team, workingPlay):
    '''positions middie'''
    if not workingPlay.isRole(PBConstants.MIDDIE):
        team.subRoleSwitchTime = -1
    workingPlay.setRole(PBConstants.MIDDIE)
    if (workingPlay.formation == PBConstants.TWO_DUB_D or
        workingPlay.formation == PBConstants.THREE_DUB_D):
        subRoleOnDeck = PBConstants.DUB_D_MIDDIE
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pDubDMiddie(team, workingPlay)
    elif (team.brain.ball.x > NogginConstants.CENTER_FIELD_X):
        subRoleOnDeck = PBConstants.DEFENSIVE_MIDDIE
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pDefensiveMiddie(team, workingPlay)
    else:
        subRoleOnDeck = PBConstants.OFFENSIVE_MIDDIE
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pOffensiveMiddie(team, workingPlay)
