/**
 * Class to handle team communications.
 * @author Wils Dawson and Josh Zalinger 4/30/12
 */

#ifndef TeamConnect_H
#define TeamConnect_H

#include "TeamMember.h"
#include "UDPSocket.h"

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
	 * @param burst:  The number of copies of this packet we should send.
	 *                If burst is less than 1, we won't send anything.
	 */
	void send(int player, int burst);

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
	TeamMember* getTeamMate(int player){return team[player-1];}

private:
	/**
	 * Sets up the socket to be used
	 */
	void setUpSocket();

	/**
	 * Builds the packet header
	 * @param packet: Pointer to the beginning of the packet.
	 * @param robot:  Pointer to the TeamMember object.
	 * @effect:       Header portion of the packet will be
	 *                built. 'packet' will be preserved.
	 * @effect:       Updates sequence number for TeamMember.
	 * @return:       Float pointer to first byte after header.
	 */
	float* buildHeader(char* packet, TeamMember* robot);

	/**
	 * Verifies the packet we received from the socket.
	 * @param packet: Pointer to the packet.
	 * @param player: The player we want packets from.
	 *                0 if we don't care.
	 * @return:       Player number we received from.
	 *                0 for failure.
	 */
	int verify(char* packet, int player);

	/**
	 * Verifies the packet header information.
	 * @param header: Pointer to the packet header struct.
	 * @return:       true for success, false for error.
	 */
	bool verifyHeader(char* header);

	TeamMember* team[NUM_PLAYERS_PER_TEAM];
	UDPSocket*  socket;
	int teamNumber;
};
#endif
