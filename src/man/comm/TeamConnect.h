/**
 * Class to handle team communications.
 * @author Wils Dawson and Josh Zalinger 4/30/12
 */

#ifndef TeamConnect_H
#define TeamConnect_H

#include "TeamMember.h"

//#include "Common.h"  
static const int NUM_PLAYERS_PER_TEAM = 4;  
#define UNIQUE_ID "B"  
#define TEAM_PORT 4000  

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
	 * @param player: The player number whose information we want.
	 *                If 0, recieve any player number.
	 */
	void receive(int player);

	/**
	 * Gets the TeamMember with the given number.
	 * @param player: The player number of the TeamMember.
	 * @return:       Pointer to the TeamMember.
	 */
	TeamMember* getTeamMate(int player){return &team[player-1];}

private:
	/**
	 * Sets up the socket to be used
	 */
	void setUpSocket();
	/**
	 * Verifies the packet we received from the socket.
	 * @param packet: Pointer to the packet.
	 * @return:       true for success, false for error.
	 */
	bool verify(char* packet);

	/**
	 * Verifies the packet header information.
	 * @param header: Pointer to the packet header struct.
	 * @return:       true for success, false for error.
	 */
	bool verifyHeader(char* header);

	/**
	 * Parses the packet information and updates the team member.
	 * @param packet: Pointer to the packet.
	 */
	void parsePacket(char* packet);

	TeamMember* team[NUM_PLAYERS_PER_TEAM];
	UDPSocket*  socket;
};
#endif
