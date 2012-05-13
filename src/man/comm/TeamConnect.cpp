/**
 * Class to handle team communications.
 * @author Josh Zalinger and Wils Dawson 4/30/12
 */

#include "TeamConnect.h"

#include <unistd.h>
#include <iostream>
#include <string>

#include "MulticastConfig.h"

//#include "commconfig.h"  
#define DEBUG_COMM  
static const int TEAM_PORT = 4000;  
#define UNIQUE_ID "B"  
static const int NUM_HEADER_BYTES = 16;

TeamConnect::TeamConnect()
{
	for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
	{
		team[i] = new TeamMember(i+1);
	}

	socket = new UDPSocket();
	setUpSocket();
}

TeamConnect::~TeamConnect()
{
	for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
	{
		delete team[i];
	}

	delete socket;
}

void TeamConnect::setUpSocket()
{
	socket->setBlocking(false);
	socket->setBroadcast(false);
	socket->bind("", TEAM_PORT); // listen for anything on our port.

	std::string ipTarget = "255.255.255.255";
	static char buf[100] = {0};
	if (!buf[0])
	{
		std::string name;

		if (gethostname(buf, sizeof(buf)) < 0)
		{
			std::cerr << "\nError getting hostname in TeamConnect::setUpSocket()"
					  << std::endl;
			goto end;
		}
		else
		{
			name = buf;
			name = name.erase(name.find('.')-1);
		}
		for (int i = 0; i < NUM_ROBOTS; ++i)
		{
			if (robotIPs[i].name.compare(name) == 0)
			{
				ipTarget = robotIPs[i].ip;
				break;
			}
			if (i == NUM_ROBOTS-1)
			{
				std::cerr << "\nError finding IP for hostname in "
						  << "TeamConnect::setUpSocket()" << std::endl;
				goto end;
			}
		}
		ipTarget = "239" + ipTarget.substr(ipTarget.find('.'));
	}

end:
	socket->setTarget(ipTarget.c_str(), TEAM_PORT);

	//join team's multicast...
	for (int i = 0; i < NUM_ROBOTS; ++i)
	{
		socket->joinMulticast(robotIPs[i].ip.c_str());
	}
}

void TeamConnect::send(int player, int burst = 1)
{
	TeamMember* robot = team[player-1];

	char packet[NUM_HEADER_BYTES + TeamMember::sizeOfData()];

	float* payload = buildHeader(&packet[0], robot);

	robot->generatePacket(payload);

	for (int i = 0; i < burst; ++i)
	{
		socket->sendToTarget(&packet[0], sizeof(packet));
	}
}

float* TeamConnect::buildHeader(char* packet, TeamMember* robot)
{
	char* cptr = packet;  // Preserve packet pointer for caller.

	*cptr = *UNIQUE_ID;
	cptr += sizeof(UNIQUE_ID);  // Advance pointer.

	*cptr   = (char)teamNumber;
	*++cptr = (char)robot->playerNumber();

	int* iptr = (int*)++cptr;
	int sn = robot->lastSeqNum() + 1;
	*iptr = sn;
	robot->setLastSeqNum(sn);

	llong* lptr = (llong*)++iptr;
	//*lptr = CommTime->timeStamp();
	*lptr = 0;
	lptr += sizeof(llong);

	float* fptr = (float*)++lptr;
	return fptr;
}

void TeamConnect::receive(int player)
{
	char packet[NUM_HEADER_BYTES + TeamMember::sizeOfData()];

	socket->receive(&packet[0], sizeof(packet));

	int playerNumber = verify(&packet[0], player);

	float* payload = (float*)(&packet[0] + NUM_HEADER_BYTES);
	TeamMember* robot = team[playerNumber -1];

	robot->update(payload);
}

int TeamConnect::verify(char* packet, int player)
{
	if (!verifyHeader(packet))
		return 0;

	// get pointer after UNIQUE_ID and teamNumber
	char* cptr = packet + sizeof(UNIQUE_ID) + 1;

	int playerNum = (int)*cptr;
	if (playerNum < 0 || playerNum > NUM_PLAYERS_PER_TEAM)
	{
#ifdef DEBUG_COMM
		std::cout << "Received packet with bad playerNumber"
				  << " in TeamConnect::verify()" << std::endl;
#endif
		return 0;
	}

	// if we care about who we recieve from:
	if (player != 0 && player != playerNum)
	{
#ifdef DEBUG_COMM
		std::cout << "Received packet with unwanted playerNumber"
				  << " in TeamConnect::verify()" << std::endl;
#endif
		return 0;
	}

	int* iptr = (int*)++cptr;
	int seqNumber = *iptr;

	TeamMember* robot = team[playerNum-1];
	if (seqNumber <= robot->lastSeqNum())
	{
#ifdef DEBUG_COMM
		std::cout << "Received packet with old sequenceNumber"
				  << " in TeamConnect::verify()" << std::endl;
#endif
		return 0;
	}

	// Success! Update seqNum and timeStamp and parse!
	robot->updateSequenceNumber(seqNumber);

	llong* lptr = (llong*)++iptr;
	llong ts = *lptr;
	robot->setLastPacketTime(ts);
	return playerNum;
}

bool TeamConnect::verifyHeader(char* header)
{
	char* ptr = header;

	const char* uID = UNIQUE_ID;
	// memcmp returns 0 on equal.
	if (memcmp(ptr, uID, sizeof(UNIQUE_ID)))
	{
#ifdef DEBUG_COMM
		std::cout << "Received packet with bad ID"
				  << " in TeamConnect::verifyHeader()" << std::endl;
#endif
		return false;
	}
	ptr += sizeof(UNIQUE_ID);

	// Assumes teamNumber can fit in one byte.
	if (memcmp(ptr, &teamNumber, 1))
	{
#ifdef DEBUG_COMM
		std::cout << "Received packet with bad teamNumber"
				  << " in TeamConnect::verifyHeader()" << std::endl;
#endif
		return false;
	}
	return true;
}
