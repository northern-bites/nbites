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
     * Constructor
     */
    GameData(int teamNumber, int playerNumber);

    /**
     * @effect: Sets up the control data to a generic state
     */
    void setUpControl();

    /**
     * @effect: Sets the control data to memory at pointer location
     */
    void setControl(char* p);

    /**
     * The following methods return the important values stored in
     * the RoboCupGameControlData structure.
     * The values it returns are consistent with the constants in
     * the "RoboCupGameControlData.h" file.
     */

    int  currentState()  {return control.state;}
    void advanceState();
    void initial()       {control.state = STATE_INITIAL;}

    bool firstHalf()     {return control.firstHalf;}

    int  myTeamColor();
    void switchTeams();

    bool ourKickoff();
    void toggleKickoff();

    int  secondState()   {return control.secondaryState;}

    int  dropInTeam()    {return control.dropInTeam;}

    int  dropInTime()    {return control.dropInTime;}

    int  timeRemaining() {return control.secsRemaining;}

    int  ourScore();

    int  theirScore();

    int  goalDifferential() {return ourScore() - theirScore();}

    /**
     * @return: number of penalized robots for the given team.
     */
    int numOurPenalizedRobots();
    int numTheirPenalizedRobots();
    int numPenalizedRobots(int team);

    /**
     * @return: time in seconds until a specified player is unpenalized
     */
    int timeUntilOurPlayerUnpened(int player);
    int timeUntilTheirPlayerUnpened(int player);
    int timeUntilPlayerUnpened(int team, int player);

    /**
     * @return: time in seconds until the next player will be available.
     */
    int timeUntilOurNextUnpened();
    int timeUntilTheirNextUnpened();
    int timeUntilNextUnpened(int team);

    /**
     * @return: time in seconds until the team is full strength.
     */
    int timeUntilWeFullStrength();
    int timeUntilTheyFullStrength();
    int timeUntilFullStrength(int team);

    int  checkPenaltyForPlayer(int team, int player);
    bool isOurPlayerPenalized(int player) {return checkPenaltyForPlayer(myTeamNumber(),
                                                                        player);}

    void setMyTeamNumber(int tn){_myTeamNumber = tn;}
    int  myTeamNumber() {return _myTeamNumber;}

    const char* toString();

private:
    GameData(); // Disallow default constructor

    int  myTeamIndex();
    int  theirTeamIndex();

    int   _myTeamNumber;
    int _myPlayerNumber;

    RoboCupGameControlData control;
};

#endif
