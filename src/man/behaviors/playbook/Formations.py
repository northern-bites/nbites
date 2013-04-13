from . import Roles
from . import SubRoles
from . import PBConstants

#### No Field Players ####

def fNoFieldPlayers(team, workingPlay):
    '''goalie'''
    workingPlay.setFormation(PBConstants.NO_FIELD_PLAYERS)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)


#### One Field Player ####

def fOneField(team, workingPlay):
    '''goalie, chaser'''
    workingPlay.setFormation(PBConstants.ONE_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rChaser(team, workingPlay)

def fOneFieldD(team, workingPlay):
    '''goalie, defender'''
    workingPlay.setFormation(PBConstants.ONE_FIELD_D)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rDefender(team, workingPlay)


#### Two Field Players ####

def fTwoField(team, workingPlay):
    '''goalie, chaser, defender'''
    workingPlay.setFormation(PBConstants.TWO_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.determineChaser(workingPlay)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            Roles.rDefender(team, workingPlay)

def fNeutralTwoField(team, workingPlay):
    '''goalie, chaser, middie'''
    workingPlay.setFormation(PBConstants.NEUTRAL_TWO_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.determineChaser(workingPlay)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            Roles.rMiddie(team, workingPlay)

def fTwoDubD(team, workingPlay):
    '''goalie(probably chaser), two defenders'''
    workingPlay.setFormation(PBConstants.TWO_DUB_D)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        Roles.rDefenderDubD(team, workingPlay)

def fTwoZoneD(team, workingPlay):
    '''goalie, chaser, defender'''
    workingPlay.setFormation(PBConstants.TWO_ZONE_D)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.getForward(team.activeFieldPlayers)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            Roles.rDefender(team, workingPlay)

def fTwoZoneO(team, workingPlay):
    '''goalie, chaser, offender'''
    workingPlay.setFormation(PBConstants.TWO_ZONE_O)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.getBack(team.activeFieldPlayers)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            Roles.rOffender(team, workingPlay)


#### Three Field Players ####

def fThreeField(team, workingPlay):
    '''goalie, chaser, offender, defender'''
    workingPlay.setFormation(PBConstants.THREE_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.determineChaser(workingPlay)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            otherMates = team.getOtherActiveFieldPlayers([chaser_mate.playerNumber])
            forward = team.getForward(otherMates)
            if forward.playerNumber == team.brain.my.playerNumber:
                Roles.rOffender(team, workingPlay)
            else:
                Roles.rDefender(team, workingPlay)

def fNeutralOThreeField(team, workingPlay):
    '''goalie, chaser, offender, middie'''
    workingPlay.setFormation(PBConstants.NEUTRAL_O_THREE_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.determineChaser(workingPlay)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            otherMates = team.getOtherActiveFieldPlayers([chaser_mate.playerNumber])
            forward = team.getForward(otherMates)
            if forward.playerNumber == team.brain.my.playerNumber:
                Roles.rOffender(team, workingPlay)
            else:
                Roles.rMiddie(team, workingPlay)

def fNeutralDThreeField(team, workingPlay):
    '''goalie, chaser, defender, middie'''
    workingPlay.setFormation(PBConstants.NEUTRAL_D_THREE_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.determineChaser(workingPlay)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            otherMates = team.getOtherActiveFieldPlayers([chaser_mate.playerNumber])
            forward = team.getForward(otherMates)
            if forward.playerNumber == team.brain.my.playerNumber:
                Roles.rMiddie(team, workingPlay)
            else:
                Roles.rDefender(team, workingPlay)

def fThreeDubD(team, workingPlay):
    '''goalie(probably chaser), two defenders, middie'''
    workingPlay.setFormation(PBConstants.THREE_DUB_D)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        forward = team.getForward(team.activeFieldPlayers)
        if forward.playerNumber == team.brain.my.playerNumber:
            Roles.rMiddie(team, workingPlay)
        else:
            Roles.rDefenderDubD(team, workingPlay)


#### Four Field Players ####

def fFourField(team, workingPlay):
    '''goalie, chaser, defender, offender, middie'''
    # Used as our pull goalie formation (2011)
    workingPlay.setFormation(PBConstants.FOUR_FIELD)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    else:
        chaser_mate = team.determineChaser(workingPlay)
        if chaser_mate.playerNumber == team.brain.my.playerNumber:
            Roles.rChaser(team, workingPlay)
        else:
            otherMates = team.getOtherActiveFieldPlayers(chaser_mate.playerNumber)
            forward = team.getForward(otherMates)
            back = team.getBack(otherMates)
            if forward.playerNumber == team.brain.my.playerNumber:
                Roles.rOffender(team, workingPlay)
            elif back.playerNumber == team.brain.my.playerNumber:
                Roles.rDefender(team, workingPlay)
            else:
                Roles.rMiddie(team, workingPlay)


#### Special Formations ####

def fKickoff(team, workingPlay):
    '''goalie, chaser, defender'''
    #roles are assigned entirely by player #
    workingPlay.setFormation(PBConstants.KICKOFF_FORMATION)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    elif team.numActiveFieldPlayers == 1:
        Roles.rChaser(team, workingPlay)
    elif team.numActiveFieldPlayers == 2:
        highNumber = team.highestActivePlayerNumber()
        if team.me.playerNumber == highNumber:
            Roles.rChaser(team, workingPlay)
        else:
            Roles.rDefender(team, workingPlay)
    elif team.numActiveFieldPlayers == 3:
        if team.me.isDefaultChaser():
            Roles.rChaser(team, workingPlay)
        elif team.me.isDefaultDefender():
            Roles.rDefender(team, workingPlay)
        elif team.me.isDefaultOffender():
            workingPlay.setRole(PBConstants.OFFENDER)
            SubRoles.pKickoffStriker(team, workingPlay)
        elif team.me.isDefaultMiddie():
            Roles.rMiddie(team, workingPlay)

def fReady(team, workingPlay):
    '''pre-kickoff positions'''
    workingPlay.setFormation(PBConstants.READY_FORMATION)
    if team.me.isDefaultGoalie():
        Roles.rGoalie(team, workingPlay)
    elif team.numActiveFieldPlayers == 1:
        workingPlay.setRole(PBConstants.CHASER)
        SubRoles.pReadyChaser(team, workingPlay)
    elif team.numActiveFieldPlayers == 2:
        highNumber = team.highestActivePlayerNumber()
        if team.me.playerNumber == highNumber:
            workingPlay.setRole(PBConstants.CHASER)
            SubRoles.pReadyChaser(team, workingPlay)
        else:
            workingPlay.setRole(PBConstants.DEFENDER)
            SubRoles.pReadyDefender(team, workingPlay)
    else:
        if team.me.isDefaultChaser():
            workingPlay.setRole(PBConstants.CHASER)
            SubRoles.pReadyChaser(team, workingPlay)
        elif team.me.isDefaultDefender():
            workingPlay.setRole(PBConstants.DEFENDER)
            SubRoles.pReadyDefender(team, workingPlay)
        elif team.me.isDefaultOffender():
            workingPlay.setRole(PBConstants.OFFENDER)
            SubRoles.pReadyOffender(team, workingPlay)


#### Test Formations ####

def fTestDefender(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_DEFEND)
    if team.brain.ball.loc.x > PBConstants.S_MIDDIE_DEFENDER_THRESH:
        Roles.rMiddie(team, workingPlay)
    else:
        Roles.rDefender(team, workingPlay)

def fTestOffender(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_OFFEND)
    if team.brain.ball.loc.x < PBConstants.S_MIDDIE_OFFENDER_THRESH:
        Roles.rMiddie(team, workingPlay)
    else:
        Roles.rOffender(team, workingPlay)

def fTestMiddie(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_MIDDIE)
    Roles.rMiddie(team, workingPlay)

def fTestChaser(team, workingPlay):
    workingPlay.setFormation(PBConstants.TEST_CHASE)
    Roles.rChaser(team, workingPlay)
