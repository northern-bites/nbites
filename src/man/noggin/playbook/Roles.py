import noggin_constants as NogginConstants
from . import PBConstants
from . import SubRoles
from . import RoleTransitions as RoleTran

#######################################
# In each role, we check to see if we should switch into a particular subRole
# This prevents hysteresis. We also set the subRoleSwitchTime to a special
# value (i.e. -1) if we are in a new role because we want to reset our counter for that.


# *THE GOALIE*
# The goalie uses a different method to decide if it should switch than
# the other field players. The counters for switching for the goalie are
# built into the RoleTransitions. The goalie has 3 basic roles.  The three
# basic roles are saving, positioning, and chasing. In addition there
# are roles for when the goalie is in ready, and when the goalie has just
# left ready.

def rGoalie(team, workingPlay):
    '''The Goalie'''

    # Make sure the goalie's role is set
    if not workingPlay.isRole(PBConstants.GOALIE):
        workingPlay.setRole(PBConstants.GOALIE)

    # Goalie starts in Ready Goalie
    if (team.brain.gameController.currentState == 'gameReady' or
        team.brain.gameController.currentState =='gameSet' or
        team.brain.gameController.currentState == 'penaltyShotsGameReady' or
        team.brain.gameController.currentState == 'penaltyShotsGameSet'):
        SubRoles.pReadyGoalie(team, workingPlay)

    # Get out of ready -> Just after kick off
    elif workingPlay.isSubRole(PBConstants.READY_GOALIE):
        SubRoles.pGoalieKickOff(team, workingPlay)

    # Make sure we position when we come out of Penalized
    # ToDo: find a way to use the after penalty states
    elif workingPlay.isSubRole(PBConstants.PENALTY_SUB_ROLE):
        SubRoles.pGoalieCenter(team, workingPlay)

    # Kick off just happened or Penalty Kicking
    elif workingPlay.isSubRole(PBConstants.GOALIE_KICKOFF):
        if RoleTran.shouldPositionForSave(team):
            SubRoles.pGoalieSave(team, workingPlay)
        elif RoleTran.shouldChase(team):
            SubRoles.pGoalieChaser(team, workingPlay)
        else:
            SubRoles.pGoalieKickOff(team, workingPlay)

    # Saving
    # When saving gets more consistent going to want a
    # way to get out of saving if ball is not coming.
    elif workingPlay.isSubRole(PBConstants.GOALIE_SAVE):
        # if RoleTran.shouldNotSave(team):
        #     SubRoles.pGoalieCenter(team, workingPlay)
        if (team.brain.player.isSaving):
            SubRoles.pGoalieSave(team, workingPlay)
        elif RoleTran.shouldChase(team):
            SubRoles.pGoalieChaser(team, workingPlay)
        else:
            SubRoles.pGoalieCenter(team, workingPlay)

    # Chase
    elif workingPlay.isSubRole(PBConstants.GOALIE_CHASER):
        if team.brain.player.inKickingState:
            SubRoles.pGoalieChaser(team, workingPlay)
        elif RoleTran.shouldStopChase(team):
            SubRoles.pGoalieCenter(team, workingPlay)
        else:
            SubRoles.pGoalieChaser(team, workingPlay)

    # Position
    elif workingPlay.isSubRole(PBConstants.GOALIE_CENTER):
        if RoleTran.shouldPositionForSave(team):
            SubRoles.pGoalieSave(team, workingPlay)
        elif RoleTran.shouldChase(team):
            SubRoles.pGoalieChaser(team, workingPlay)
        else:
            SubRoles.pGoalieCenter(team, workingPlay)

def rChaser(team, workingPlay):
    '''sets current position for chaser since positioning is done by player'''
    if not workingPlay.isRole(PBConstants.CHASER):
        team.subRoleSwitchTime = 0
    workingPlay.setRole(PBConstants.CHASER)
    # Note: Don't use shouldSwitchSubRole here. Go for the ball immediately.
    workingPlay.setSubRole(PBConstants.CHASE_NORMAL)
    pos = (team.brain.my.x,team.brain.my.y, team.brain.ball.loc.heading)
    workingPlay.setPosition(pos)

def rDefender(team, workingPlay):
    '''gets positioning for defender'''
    if not workingPlay.isRole(PBConstants.DEFENDER):
        team.subRoleSwitchTime = -1
    workingPlay.setRole(PBConstants.DEFENDER)
    if team.brain.ball.loc.x < PBConstants.SWEEPER_X_THRESH:
        subRoleOnDeck = PBConstants.SWEEPER
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pSweeper(team, workingPlay)
    elif team.brain.ball.loc.x < PBConstants.CENTER_BACK_X_THRESH:
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

    ballX = team.brain.ball.loc.x
    ballY = team.brain.ball.loc.y
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
    elif (team.brain.ball.loc.x > NogginConstants.CENTER_FIELD_X):
        subRoleOnDeck = PBConstants.DEFENSIVE_MIDDIE
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pDefensiveMiddie(team, workingPlay)
    else:
        subRoleOnDeck = PBConstants.OFFENSIVE_MIDDIE
        if team.shouldSwitchSubRole(subRoleOnDeck, workingPlay):
            SubRoles.pOffensiveMiddie(team, workingPlay)
