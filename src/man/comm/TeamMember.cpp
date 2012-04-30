/**
 * Encapsulates the information communicated to us from our team members.
 * @author Wils Dawson 4/26/12
 */

#include "TeamMember.h"

TeamMember::TeamMember(int num)
	: _playerNumber(num)
{
}

int TeamMember::update(llong time, int seqNum, float* packet)
{
	setLastPacketTime(time);
	setActive(true);

	float* ptr = packet;

	setMyX(*ptr);
	setMyY(*++ptr);
	setMyH(*++ptr);
	setMyXUncert(*++ptr);
	setMyYUncert(*++ptr);
	setMyHUncert(*++ptr);
	setBallDist(*++ptr);
	setBallBearing(*++ptr);
	setBallDistUncert(*++ptr);
	setBallBearingUncert(*++ptr);
	setChaseTime(*++ptr);
	setRole(*++ptr);
	setSubRole(*++ptr);

	int missedPackets = seqNum - lastSeqNum() - 1;
	setLastSeqNum(seqNum);
	return missedPackets;
}
