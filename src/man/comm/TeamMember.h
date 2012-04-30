/**
 * Encapsulates the information communicated to us from our team members.
 * @author Wils Dawson 4/26/12
 */

#ifndef TeamMember_H
#define TeamMember_H

#include "CommDef.h"

class TeamMember
{
public:
	/**
	 * Constructor
	 * @param num: The player number for this Team Member.
	 */
	TeamMember(int num);

	/**
	 * Updates the Object with the time the packet was received
	 * and with the contents of the packet.
	 * @param time:   Time that this packet was received.
	 * @param packet: float values for this packet.
	 */
	void update(llong time, float* packet);

	/***********************
	 * Getters and Setters *
	 ***********************/
	void setPlayerNumber(int num) {_playerNumber = num;}
	int  playerNumber() {return _playerNumber;}

	void  setMyX(float x) {_myX = x;}
	float myX() {return _myX;}

	void  setMyY(float y) {_myY = y;}
	float myY() {return _myY;}

	void  setMyH(float h) {_myH = h;}
	float myH() {return _myH;}

	void  setMyXUncert(float xU) {_myXUncert = xU;}
	float myXUncert() {return _myXUncert;}

	void  setMyYUncert(float yU) {_myYUncert = yU;}
	float myYUncert() {return _myYUncert;}

	void  setMyHUncert(float hU) {_myHUncert = hU;}
	float myHUncert() {return _myHUncert;}

	void  setBallDist(float bd) {_ballDist = bd;}
	float ballDist() {return _ballDist;}

	void  setBallBearing(float bb) {_ballBearing = bb;}
	float ballBearing() {return _ballBearing;}

	void  setBallDistUncert(float bdU) {_ballDistUncert = bdU;}
	float ballDistUncert() {return _ballDistUncert;}

	void  setBallBearingUncert(float bbU) {_ballBearingUncert = bbU;}
	float ballBearingUncert() {return _ballBearingUncert;}

	void  setChaseTime(float ct) {_chaseTime = ct;}
	float chaseTime() {return _chaseTime;}

	void  setRole(float r) {_role = r;}
	float role() {return _role;}

	void  setSubRole(float sr) {_subRole = sr;}
	float subRole() {return _subRole;}

	void  setLastPacketTime(llong t) {_lastPacketTime = t;}
	llong lastPacketTime() {return _lastPacketTime;}

	void  setActive(bool a) {_active = a;}
	bool  active() {return _active;}

private:
	TeamMember();            // Disallow default constructor.

	int   _playerNumber;      // The player number
	float _myX;               // The global x location on the field.
	float _myY;               // The global y location on the field.
	float _myH;               // The global heading on the field.
	float _myXUncert;         // Uncertainty in the x value.
	float _myYUncert;         // Uncertainty in the y value.
	float _myHUncert;         // Uncertainty in the h value.
	float _ballDist;          // Relative distance to the ball.
	float _ballBearing;       // Relative bearing to the ball.
	float _ballDistUncert;    // Uncertainty in the dist value.
	float _ballBearingUncert; // Uncertainty in the bearing value.
	float _chaseTime;         // Estimate of how long to reach the ball.
	float _role;              // Playbook Role
	float _subRole;           // Playbook SubRole

	llong _lastPacketTime;    // Time that the last packet was received.
	bool  _active;            // Is the robot active.
};
#endif
