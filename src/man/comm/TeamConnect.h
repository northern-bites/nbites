/**
 * Class to handle team communications.
 * @author Wils Dawson 4/30/12
 */

#ifndef TeamConnect_H
#define TeamConnect_H

//#include "Common.h"  
static const int NUM_PLAYERS_PER_TEAM = 4;  

class TeamConnect
{
public:
	/**
	 * Constructor.
	 */
	TeamConnect();

	/**
	 * Destructor.
	 */
	~TeamConnect();

	/**
	 * Signal to send our current information to the team mates.
	 * @param player: The player number whose information we should send.
	 */
	void send(int player);

	/**
	 * Signal to receive any information from the socket.
	 * @param received: Pointer to the int for successful receptions.
	 * @param dropped:  Pointer to the int for dropped/delayed receptions.
	 * @return:         Received and dropped/delayed numbers returned by reference.
	 */
	void receive(int* received, int* dropped);

	/**
	 * Gets the TeamMember with the given number.
	 * @param player: The player number of the TeamMember.
	 * @return:       The TeamMember.
	 */
	TeamMember* getTeamMate(int player){return &team[player-1];}

private:
	TeamMember team[NUM_PLAYERS_PER_TEAM];
	UDPSocket  socket;
};
#endif
