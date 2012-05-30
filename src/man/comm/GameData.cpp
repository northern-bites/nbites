/**
 * Encapsulates RoboCupGameControlData in an object.
 * Provides convenient methods for accessing and interpreting data.
 * If the GameController's protocol ever changes significantly, this
 * class may have to be changed as well.
 * @author Wils Dawson 5/29/2012
 */

#include "GameData.h"

#include <string>  
#include <sstream>  

bool GameData::ourKickoff()
{
	return true;
}

int GameData::ourScore()
{
	return 10;
}

int GameData::theirScore()
{
	return 0;
}

int GameData::numOurPenalizedRobots()
{
	return numPenalizedRobots(0);
}

int GameData::numTheirPenalizedRobots()
{
	return numPenalizedRobots(1);
}

int GameData::numPenalizedRobots(int team)
{
	return 0;
}

int GameData::timeUntilOurPlayerUnpened(int player)
{
	return 0;
}

int GameData::timeUntilTheirPlayerUnpened(int player)
{
	return 0;
}

int GameData::timeUntilPlayerUnpened(int team, int player)
{
	return 0;
}

int GameData::timeUntilOurNextUnpened()
{
	return 0;
}

int GameData::timeUntilTheirNextUnpened()
{
	return 0;
}

int GameData::timeUntilNextUnpened(int team)
{
	return 0;
}

int GameData::timeUntilWeFullStrength()
{
	return 0;
}

int GameData::timeUntilTheyFullStrength()
{
	return 0;
}

int GameData::timeUntilFullStrength(int team)
{
	return 0;
}

int GameData::myTeamColor()
{
	return 0;
}

char* GameData::toString()
{
	std::string d;

	std::stringstream data;

	data << "=====Game Data=====\n";

	data << "Header:\t";
	data << control.header;
	data << "\n";

	data << "Version:\t";
	data << control.version;
	data << "\n";

	data << "Players Per Team:\t";
	data << control.playersPerTeam;
	data << "\n";

	data << "State:\t";
	data << control.state;
	data << "\n";

	data << "First Half:\t";
	data << control.firstHalf;
	data << "\n";

	data << "Kickoff Team:\t";
	data << control.kickOffTeam;
	data << "\n";

	data << "Second State:\t";
	data << control.secondaryState;
	data << "\n";

	data << "Drop In Team:\t";
	data << control.dropInTeam;
	data << "\n";

	data << "Drop In Time:\t";
	data << control.state;
	data << "\n";

	data << "Remaining:\t";
	data << control.secsRemaining;
	data << "\n";

	data << "Team 0:\n";

	data << "\tTeam Number:\t";
	data << control.teams[0].teamNumber;
	data << "\n";

	data << "\tTeam Color:\t";
	data << control.teams[0].teamColour;
	data << "\n";

	data << "\tGoal Color:\t";
	data << control.teams[0].goalColour;
	data << "\n";

	data << "\tScore:\t";
	data << control.teams[0].score;
	data << "\n";

	data << "\t\tRobot 1:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[0].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[0].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 2:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[1].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[1].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 3:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[2].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[2].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 4:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[3].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[3].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 5:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[4].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[4].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 6:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[5].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[5].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 7:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[6].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[6].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 8:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[7].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[7].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 9:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[8].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[8].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 10:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[9].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[9].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 11:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[0].players[10].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[0].players[10].secsTillUnpenalised;
	data << "\n";


/*l kjdalkfj alksdj flajdlfkj ewaoifjewaoiufhds ahfadshfouahdf
lasdfjoawei goadighaoghsdkljghids ahfg
aoidegjfoaiehg eoihgaoiewjgfoisdjf sdhfoiadgoijewg
ilajgoiejgoeai gjewoigjfa sdpf jdsfgeig eadi fjdjife */

	data << "Team 1:\n";

	data << "\tTeam Number:\t";
	data << control.teams[1].teamNumber;
	data << "\n";

	data << "\tTeam Color:\t";
	data << control.teams[1].teamColour;
	data << "\n";

	data << "\tGoal Color:\t";
	data << control.teams[1].goalColour;
	data << "\n";

	data << "\tScore:\t";
	data << control.teams[1].score;
	data << "\n";

	data << "\t\tRobot 1:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[0].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[0].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 2:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[1].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[1].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 3:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[2].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[2].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 4:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[3].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[3].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 5:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[4].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[4].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 6:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[5].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[5].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 7:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[6].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[6].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 8:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[7].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[7].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 9:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[8].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[8].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 10:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[9].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[9].secsTillUnpenalised;
	data << "\n";

	data << "\t\tRobot 11:\n";

	data << "\t\tPenalty:\t";
	data << control.teams[1].players[10].penalty;
	data << "\n";

	data << "\t\tSeconds Left:\t";
	data << control.teams[1].players[10].secsTillUnpenalised;
	data << "\n";

	d = data.str();
}
