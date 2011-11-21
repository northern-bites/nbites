
#include "RobotFinder.h"
#include "CommDef.h"

#include <iostream>
#include <time.h>

using namespace std;

namespace qtool {
namespace remote {

RobotFinder::RobotFinder() {
    udpSocket.bind(TOOL_PORT);
    refresh();
}

void RobotFinder::refresh() {
    remoteRobots.clear();
    broadcastDiscoveryMessage();
    sleep(1);
    checkForAnswerMessage();
    emit refreshedRemoteRobotList();
}

void RobotFinder::broadcastDiscoveryMessage() {
    udpSocket.writeDatagram(TOOL_REQUEST_MSG, TOOL_REQUEST_LEN,
                            QHostAddress(QHostAddress::Broadcast), UDP_PORT);
}

void RobotFinder::checkForAnswerMessage() {
    while(udpSocket.hasPendingDatagrams()) {
        qint64 datagram_size = udpSocket.pendingDatagramSize();
        QHostAddress datagram_source;
        quint16 datagram_port;
        char* data = new char[datagram_size];
        udpSocket.readDatagram(data, datagram_size,
                &datagram_source, &datagram_port);
        if (strncmp(data, TOOL_ACCEPT_MSG, TOOL_ACCEPT_LEN) == 0) {
            RemoteRobot newRemoteRobot(datagram_source,
                    string(data + TOOL_ACCEPT_NAME_OFFSET));
            addRemoteRobot(newRemoteRobot);
        }
    }
}

void RobotFinder::addRemoteRobot(RemoteRobot& remoteRobot) {
    remoteRobots.push_back(remoteRobot);
}

}
}
