#include <vector>
#include "BotLocs.h"
#include "CommDef.h"
#include <QtNetwork/QUdpSocket>
#include <QByteArray>
#include <QtNetwork/QHostAddress>
#include <QtDebug>
#include "NBMath.h"

namespace qtool {
namespace viewer {

int numBots = 0;

BotLocs::BotLocs(QObject* parent) : QObject(parent) {
}

void BotLocs::startListening(){
    udpSocket.bind(QHostAddress("0.0.0.0"), TEAM_PORT);
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

        if(datagram_size==72){
            Bot newBot;

            data += 2;  // get past unique ID
            newBot.teamNum = (int)*data;
            newBot.playerNum = (int)*(++data);
            newBot.teamColor = 0; // HACK! we don't communicate color
                                  // we should assign teams random colors
                                  // until we get a GC packet that
                                  // says otherwise. Comm Module needed.

            data += 4;   // get past seq. num

            float* floatdata = (float*) data;
            newBot.address = datagram_source;
            newBot.xPos = floatdata[0];
            newBot.yPos = floatdata[1];
            newBot.heading = floatdata[2];
            newBot.xUncert = floatdata[3];
            newBot.yUncert = floatdata[4];
            newBot.headingUncert = floatdata[5];
            newBot.ballOn = floatdata[6];
            newBot.ballDist = floatdata[7];
            newBot.ballBearing = floatdata[8];
            newBot.ballDistUncert = floatdata[9];
            newBot.ballBearingUncert = floatdata[10];

            //kill the previous instance of this robot in the array
            for(int i = 0; i < botPositions.size(); i++){
                if(botPositions[i].address == newBot.address){
                    botPositions.erase(botPositions.begin() + i);
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

        emit newRobotLocation();
    }
}

int BotLocs::getX(int i){
    return (int)botPositions[i].xPos;
}
int BotLocs::getY(int i){
    return (int)botPositions[i].yPos;
}
int BotLocs::getHeading(int i){
    return (int)botPositions[i].heading;
}
int BotLocs::getXUncert(int i){
    return (int)botPositions[i].xUncert;
}
int BotLocs::getYUncert(int i){
    return (int)botPositions[i].yUncert;
}
int BotLocs::getheadUncert(int i){
    return (int)botPositions[i].headingUncert;
}
int BotLocs::getBallX(int i){
    return (int)getX(i)+botPositions[i].ballDist*std::cos(botPositions[i].heading+
                                                     botPositions[i].ballBearing);
}
int BotLocs::getBallY(int i){
    return (int)getY(i)+botPositions[i].ballDist*std::sin(botPositions[i].heading+
                                                     botPositions[i].ballBearing);
}
int BotLocs::getBallXUncert(int i){
    int ballX = getBallX(i);
    int ballY = getBallY(i);

    int errX = getX(i)+(botPositions[i].ballDist+botPositions[i].ballDistUncert)*
        std::cos(botPositions[i].heading+
                 botPositions[i].headingUncert+
                 botPositions[i].ballBearing+
                 botPositions[i].ballBearingUncert);
    int errY = getY(i)+(botPositions[i].ballDist+botPositions[i].ballDistUncert)*
        std::sin(botPositions[i].heading+
                 botPositions[i].headingUncert+
                 botPositions[i].ballBearing+
                 botPositions[i].ballBearingUncert);
    return (int)std::sqrt((errX-ballX)*(errX-ballX)+(errY-ballY)*(errY-ballY))+
        getXUncert(i);
}
int BotLocs::getBallYUncert(int i){
    int ballX = getBallX(i);
    int ballY = getBallY(i);

    int errX = getX(i)+(botPositions[i].ballDist+botPositions[i].ballDistUncert)*
        std::cos(botPositions[i].heading+
                 botPositions[i].headingUncert+
                 botPositions[i].ballBearing+
                 botPositions[i].ballBearingUncert);
    int errY = getY(i)+(botPositions[i].ballDist+botPositions[i].ballDistUncert)*
        std::sin(botPositions[i].heading+
                 botPositions[i].headingUncert+
                 botPositions[i].ballBearing+
                 botPositions[i].ballBearingUncert);
    return (int)std::sqrt((errX-ballX)*(errX-ballX)+(errY-ballY)*(errY-ballY))+
        getYUncert(i);
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
