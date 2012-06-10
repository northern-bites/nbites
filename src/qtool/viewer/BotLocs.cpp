#include <vector>
#include "BotLocs.h"
#include "CommDef.h"
#include <QtNetwork/QUdpSocket>
#include <QByteArray>
#include <QtNetwork/QHostAddress>
#include <QtDebug>

namespace qtool {
namespace viewer {

int numBots = 1;

BotLocs::BotLocs(){
	Bot b;
	b.address = "0.0.0.0";
	botPositions.push_back(b); //make a fake robot to prevent segfaults
}

void BotLocs::startListening(){
	udpSocket.bind(QHostAddress("0.0.0.0"), UDP_PORT);
	connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(readSocket()));
}

void BotLocs::stopListening(){
	udpSocket.close();
}

void BotLocs::readSocket(){
	while(udpSocket.hasPendingDatagrams()){

		qint64 datagram_size = udpSocket.pendingDatagramSize();
		QHostAddress datagram_source;
		quint16 datagram_port;
		char* data = new char[datagram_size];

		udpSocket.readDatagram(data, datagram_size, &datagram_source, &datagram_port);

		if(datagram_size==112){ //this needs to be here to ignore
			//the 60-byte discovery messages
			CommPacketHeader* head = (CommPacketHeader*)data;
			data+=sizeof(CommPacketHeader); //cut off the header bytes
			float* floatdata = (float*) data;
			Bot newBot;

			newBot.address = datagram_source;
			newBot.xPos = floatdata[0];
			newBot.yPos = floatdata[1];
			newBot.heading = floatdata[2];
			newBot.xUncert = floatdata[3];
			newBot.yUncert = floatdata[4];
			newBot.headingUncert = floatdata[5];
			newBot.xBall = floatdata[6];
			newBot.yBall = floatdata[7];
			newBot.xBallUncert = floatdata[8];
			newBot.yBallUncert = floatdata[9];
			newBot.teamNum = head->team;
			newBot.teamColor = head->color;
			newBot.playerNum = head->player;

			//kill the previous instance of this robot in the array
			for(int i = 0; i < botPositions.size(); i++){
				if(botPositions[i].address == newBot.address){
					botPositions.erase(botPositions.begin()+i);
				}
			}

			botPositions.push_back(newBot);
		}
		if (botPositions.size()>numBots)
			qDebug()<<"Player"
				<<botPositions.back().playerNum<<"on team"
				<<botPositions.back().teamNum<<"connected.";
		else if (botPositions.size()<numBots)
			qDebug()<<"Robot connection lost,"
					<<botPositions.size()-1<<" robots still connected.";
		numBots=botPositions.size();
	}

}

int BotLocs::getX(int i){
	return botPositions[i].xPos;
}
int BotLocs::getY(int i){
	return botPositions[i].yPos;
}
int BotLocs::getHeading(int i){
	return botPositions[i].heading;
}
int BotLocs::getXUncert(int i){
	return botPositions[i].xUncert;
}
int BotLocs::getYUncert(int i){
	return botPositions[i].yUncert;
}
int BotLocs::getheadUncert(int i){
	return botPositions[i].headingUncert;
}
int BotLocs::getBallX(int i){
	return botPositions[i].xBall;
}
int BotLocs::getBallY(int i){
	return botPositions[i].yBall;
}
int BotLocs::getBallXUncert(int i){
	return botPositions[i].xBallUncert;
}
int BotLocs::getBallYUncert(int i){
	return botPositions[i].yBallUncert;
}
int BotLocs::getTeamNum(int i){
	return botPositions[i].teamNum;
}
int BotLocs::getTeamColor(int i){//0=blue, 1=red
	return botPositions[i].teamColor;
}
int BotLocs::getPlayerNum(int i){
	return botPositions[i].playerNum;
}
int BotLocs::getSize(){
	return botPositions.size();
}
}
}
