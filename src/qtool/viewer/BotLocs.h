/**
 *
 *@class Viewer
 *
 * This class contains the locations of robots
 *
 * @author Brian Jacobel
 * @date Apr 2012
 *
 */

#pragma once

#include <vector>
#include <QtNetwork/QUdpSocket>
#include "data/DataManager.h"

namespace qtool {
namespace viewer {

class BotLocs: public QObject {
	Q_OBJECT

	public:
	BotLocs();
	~BotLocs(){}
	int getX(int);
	int getY(int);
	int getHeading(int);
	int getXUncert(int);
	int getYUncert(int);
	int getheadUncert(int);
	int getBallX(int);
	int getBallY(int);
	int getBallXUncert(int);
	int getBallYUncert(int);
	int getTeam(int);
	int getPlayerNum(int);
	int getSize();
	void startListening();
	void stopListening();

private:
	struct Bot{
		QHostAddress address;
		float xPos;
		float yPos;
		float heading;
		float xUncert;
		float yUncert;
		float headingUncert;
		float xBall;
		float yBall;
		float xBallUncert;
		float yBallUncert;
		int teamColor; //0=blue,  1=red
		int playerNum;
	};
	std::vector<Bot> botPositions;
	std::vector<QHostAddress> onField;
	QUdpSocket udpSocket;

public slots:
	void readSocket();

};
}
}
