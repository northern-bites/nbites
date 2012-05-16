/**
 * Encapsulates the information communicated to us from our team members.
 * @author Wils Dawson 4/26/12
 */

#include "TeamMember.h"

TeamMember::TeamMember(int num)
	: _playerNumber(num)
{
}

void TeamMember::update(float* packet)
{
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
}

void TeamMember::generatePacket(float* packet)
{
	float* ptr = packet;

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
}
