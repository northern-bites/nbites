/**
 * Encapsulates the information communicated to us from our team members.
 * @author Wils Dawson 4/26/12
 */

#include "TeamMember.h"

TeamMember::TeamMember(int num)
	: _playerNumber(num)
{
}

unsigned int TeamMember::update(llong time, unsigned int seqNum, float* packet)
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

	unsigned int missedPackets = seqNum - lastSeqNum() - 1;
	setLastSeqNum(seqNum);
	return missedPackets;
}

float* TeamMember::generatePacket()
{
	float* ptr;
	float* packet = ptr;

	*  ptr = myX();
	*++ptr = myY();
	*++ptr = myH();
	*++ptr = myXUncert();
	*++ptr = myYUncert();
	*++ptr = myHUncert();
	*++ptr = ballDist();
	*++ptr = ballBearing();
	*++ptr = ballDistUncert();
	*++ptr = ballBearingUncert();
	*++ptr = chaseTime();
	*++ptr = role();
	*++ptr = subRole();

	return packet;
}

int TeamMember::sizeOfData()
{
	return NUM_DATA_FIELDS * sizeof(float);
}
