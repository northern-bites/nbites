/**
 * Encapsulates RoboCupGameControlData in an object.
 * Provides convenient methods for accessing and interpreting data.
 * If the GameController's protocol ever changes significantly, this
 * class may have to be changed as well.
 * @author Wils Dawson 5/29/2012
 */

#include "GameData.h"

#include <string.h>
#include <iostream>

GameData::GameData(int teamNumber, int playerNumber)
{
    _myTeamNumber = teamNumber;
    _myPlayerNumber = playerNumber;
    setUpControl();
    std::cout << "Constructed GameData" << std::endl;
}

void GameData::setUpControl()
{
    struct RoboCupGameControlData data;
    memcpy(&data.header, GAMECONTROLLER_STRUCT_HEADER, sizeof(data.header));
    data.version = GAMECONTROLLER_STRUCT_VERSION;
    data.playersPerTeam = MAX_NUM_PLAYERS;
    data.state = STATE_INITIAL;
    data.firstHalf = 1;
    data.kickOffTeam = TEAM_BLUE;
    data.secondaryState = STATE2_NORMAL;
    data.dropInTeam = TEAM_BLUE;
    data.dropInTime = 0;
    data.secsRemaining = 600;

    data.teams[TEAM_BLUE].teamNumber = (uint8)_myTeamNumber;
    data.teams[TEAM_BLUE].teamColour = TEAM_BLUE;
    data.teams[TEAM_BLUE].goalColour = GOAL_BLUE;

    data.teams[TEAM_RED].teamNumber = 0;
    data.teams[TEAM_RED].teamColour = TEAM_RED;
    data.teams[TEAM_RED].goalColour = GOAL_YELLOW;

    control = data;
}

void GameData::setControl(char* p)
{
    memcpy(&control, p, sizeof(RoboCupGameControlData));
}

void GameData::advanceState()
{
    int currentState = control.state;
    if (currentState == STATE_INITIAL)
        control.state = STATE_READY;
    else if (currentState == STATE_READY)
        control.state = STATE_SET;
    else if (currentState == STATE_SET)
        control.state = STATE_PLAYING;
    else if (currentState == STATE_PLAYING){
        RobotInfo me = control.teams[myTeamIndex()].players[_myPlayerNumber-1];
        if (me.penalty)
        {
            me.penalty = PENALTY_NONE;
            me.secsTillUnpenalised = 0;
        }
        else
        {
            me.penalty = PENALTY_MANUAL;
            me.secsTillUnpenalised = 30;
            // @TODO: Send something to a remote GC. We don't have a
            // reference to GameConnect right now, so that's hard. But!
            // with the new architecture we shouldn't be calling this
            // function from outside Comm Module anyway =D
        }
    }
}

void GameData::toggleKickoff()
{
    control.kickOffTeam = ((control.kickOffTeam == TEAM_BLUE) ?
                           TEAM_RED : TEAM_BLUE);
}

bool GameData::ourKickoff()
{
    return (control.teams[myTeamIndex()].teamColour == control.kickOffTeam);
}

int GameData::ourScore()
{
    return (int)control.teams[myTeamIndex()].score;
}

int GameData::theirScore()
{
    return (int)control.teams[theirTeamIndex()].score;
}

int GameData::numOurPenalizedRobots()
{
    return numPenalizedRobots(myTeamIndex());
}

int GameData::numTheirPenalizedRobots()
{
    return numPenalizedRobots(theirTeamIndex());
}

int GameData::numPenalizedRobots(int team)
{
    int count = 0;
    for (int player = 0; player < MAX_NUM_PLAYERS; ++player)
    {
        if(checkPenaltyForPlayer(team, player) != PENALTY_NONE)
            ++count;
    }
    return count;
}

int GameData::timeUntilOurPlayerUnpened(int player)
{
    return timeUntilPlayerUnpened(myTeamIndex(), player);
}

int GameData::timeUntilTheirPlayerUnpened(int player)
{
    return timeUntilPlayerUnpened(theirTeamIndex(), player);
}

int GameData::timeUntilPlayerUnpened(int team, int player)
{
    return (int)control.teams[team].players[player].secsTillUnpenalised;
}

int GameData::timeUntilOurNextUnpened()
{
    return timeUntilNextUnpened(myTeamIndex());
}

int GameData::timeUntilTheirNextUnpened()
{
    return timeUntilNextUnpened(theirTeamIndex());
}

int GameData::timeUntilNextUnpened(int team)
{
    int minTime = 30;
    bool penalizedRobot = false;
    for (int i = 0; i < MAX_NUM_PLAYERS; ++i)
    {
        if (checkPenaltyForPlayer(team, i) != PENALTY_NONE)
        {
            penalizedRobot = true;
            if (timeUntilPlayerUnpened(team, i) < minTime)
                minTime = timeUntilPlayerUnpened(team, i);
        }
    }
    if (!penalizedRobot)
        minTime = 0;
    return minTime;
}

int GameData::timeUntilWeFullStrength()
{
    return timeUntilFullStrength(myTeamIndex());
}

int GameData::timeUntilTheyFullStrength()
{
    return timeUntilFullStrength(theirTeamIndex());
}

int GameData::timeUntilFullStrength(int team)
{
    int maxTime = 0;
    for (int i = 0; i < MAX_NUM_PLAYERS; ++i)
    {
        if (checkPenaltyForPlayer(team, i) != PENALTY_NONE &&
            timeUntilPlayerUnpened(team, i) > maxTime)
            maxTime = timeUntilPlayerUnpened(team, i);
    }
    return maxTime;
}

int GameData::myTeamColor()
{
    return (int)control.teams[myTeamIndex()].teamColour;
}

int GameData::myTeamIndex()
{
    for (int i = 0; i < 2; ++i)
    {
        if (control.teams[i].teamNumber == _myTeamNumber)
            return i;
    }
    std::cerr << "GameData::myTeamIndex() -- not found" << std::endl;
    return -1;
}

int GameData::theirTeamIndex()
{
    for (int i = 0; i < 2; ++i)
    {
        if (control.teams[i].teamNumber != _myTeamNumber)
            return i;
    }
    std::cerr << "GameData::theirTeamIndex() -- not found" << std::endl;
    return -1;
}

int GameData::checkPenaltyForPlayer(int team, int player)
{
    return (int)control.teams[team].players[player].penalty;
}

void GameData::switchTeams()
{
    struct TeamInfo temp = control.teams[0];

    control.teams[0] = control.teams[1];
    control.teams[0].teamColour = TEAM_BLUE;
    control.teams[0].goalColour = GOAL_BLUE;

    control.teams[1] = temp;
    control.teams[1].teamColour = TEAM_RED;
    control.teams[1].goalColour = GOAL_YELLOW;
}
