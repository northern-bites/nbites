/**
 * Class to handle team communications.
 * @author Josh Zalinger and Wils Dawson 4/30/12
 */

#include "TeamConnect.h"

#include <unistd.h>
#include <string>

#include "UDPSocket.h"
#include "MulticastConfig.h"

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
		if (gethostname(buf, sizeof(buf)) < 0)
		{
			std::cerr << "\nError getting hostname in TeamConnect::setUpSocket()"
					  << endl;
			socket->setTarget(ipTarget, TEAM_PORT);
			return;
		}
		else
		{
			std::string name = &buf;
			name = name.erase(name.find('.')-1);
		}
		for (int i = 0; i < NUM_ROBOTS; ++i)
		{
			if (robotIPs[i].name.compare(name) == 0)
			{
				ipTarget = robotIPs[i].ip;
				break;
			}
			std::cerr << "\nError finding IP for hostname in "
					  << "TeamConnect::setUpSocket()" << endl;
			socket->setTarget(ipTarget, TEAM_PORT);
			return;
		}
		ipTarget = "239" + ipTarget.substr(ipTarget.find('.'));
	}

	socket->setTarget(ipTarget, TEAM_PORT);

	//join team's multicast...
	for (int i = 0; i < NUM_ROBOTS; ++i)
	{
		socket->joinMulticast(robotIPs[i].ip.c_str());
	}
}

void TeamConnect::send(int player)
{

}

void TeamConnect::receive(int player)
{

}

bool TeamConnect::verify(char* packet)
{

}

bool TeamConnect::verifyHeader(char* header)
{

}

void TeamConnect::parsePacket(char* packet)
{

}
