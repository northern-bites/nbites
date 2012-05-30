/**
 * Encapsulates RoboCupGameControlData in an object.
 * Provides convenient methods for accessing and interpreting data.
 * If the GameController's protocol ever changes significantly, this
 * class may have to be changed as well.
 * @author Wils Dawson 5/29/2012
 */

#ifndef GameData_H
#define GameData_H

#include "RoboCupGameControlData.h"

class GameData
{
public:
	/**
	 * The following methods return the important values stored in
	 * the RoboCupGameControlData structure.
	 * The values it returns are consistent with the constants in
	 * the "RoboCupGameControlData.h" file.
	 */

	int  currentState()  {return control.state;}

	bool firstHalf()     {return control.firstHalf;}

	bool ourKickoff();

	int  secondState()   {return control.secondaryState;}

	int  dropInTeam()    {return control.dropInTeam;}

	int  dropInTime()    {return control.dropInTime;}

	int  timeRemaining() {return control.secsRemaining;}

	int  ourScore();

	int  theirScore();

	void goalDifferential() {return ourScore() - theirScore();}

    /**
	 * @return: number of penalized robots for the given team.
	 */
	int numOurPenalizedRobots();
	int numTheirPenalizedRobots();
	int numPenalizedRobots(int team);

	int timeUntilOurPlayerUnpened(int player);
	int timeUntilTheirPlayerUnpened(int player);
	int timeUntilPlayerUnpened(int team, int player);

	int timeUntilOurNextUnpened();
	int timeUntilTheirNextUnpened();
	int timeUntilNextUnpened(int team);

	int timeUntilWeFullStrength();
	int timeUntilTheyFullStrength();
	int timeUntilFullStrength(int team);

	void setMyTeamNumber(int tn){_myTeamNumber = tn}
	int  myTeamNumber() {return _myTeamNumber;}

	int  myTeamColor();

	char* toString();

private:
	int   _myTeamNumber;

	RoboCupGameControlData control;
};

#endif
