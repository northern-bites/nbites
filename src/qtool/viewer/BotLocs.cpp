#include <vector>
#include "BotLocs.h"
#include "CommDef.h"
#include <QtNetwork/QUdpSocket>
#include <QByteArray>
#include <QtNetwork/QHostAddress>
#include <QtDebug>

namespace qtool {
  namespace viewer {

    int bsCounter;
    
    BotLocs::BotLocs(){    
      Bot b;
      b.address = "0.0.0.0";
      botPositions.push_back(b);
      bsCounter = 0;
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
	if(datagram_size==108){
	  //qDebug()<<"mmm, delicious packets";
	  data+=sizeof(CommPacketHeader); //cut off the header bytes
	  
	  Bot newBot;
	  newBot.address = datagram_source;	  
	  newBot.xPos = ((float*)data)[0];
	  newBot.yPos = ((float*)data)[1];
	  //data[2] is the heading, idgaf
	  newBot.xUncert = ((float*)data)[3];
	  newBot.yUncert = ((float*)data)[4];
	  
	  for(int i = 0; i < botPositions.size(); i++){
	    if(botPositions[i].address == newBot.address){
	      botPositions.erase(botPositions.begin()+i);
	    }
	  }

	  botPositions.push_back(newBot);
	  }
      }
    }

    int BotLocs::getX(int i){
      return botPositions[i].xPos;
    }
    int BotLocs::getY(int i){
      return botPositions[i].yPos;
    }
    int BotLocs::getXUncert(int i){
      return botPositions[i].xUncert;
    }
    int BotLocs::getYUncert(int i){
      return botPositions[i].yUncert;
    }
    int BotLocs::getSize(){
      return botPositions.size();
    }
  }
}
