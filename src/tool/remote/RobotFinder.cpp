
#include "RobotFinder.h"
#include "CommDef.h"

#include <iostream>
#include <time.h>

using namespace std;

namespace qtool {
namespace remote {

RobotFinder::RobotFinder() {
    udpSocket.bind(TEAM_PORT, QUdpSocket::ShareAddress);

    // Join Multicast groups
    for (int i=0; i<NUM_ROBOTS; ++i)
    {
        const QString ip = robotIPs[i].ip.c_str();
        udpSocket.joinMulticastGroup(QHostAddress(ip));
    }

    connect(&udpSocket, SIGNAL(readyRead()),
            this, SLOT(checkForMessages()));
}

void RobotFinder::refresh() {
    remoteRobots.clear();
}

void RobotFinder::checkForMessages() {
    while(udpSocket.hasPendingDatagrams()) {
        qint64 datagram_size = udpSocket.pendingDatagramSize();
        QHostAddress datagram_source;
        quint16 datagram_port;
        char* data = new char[datagram_size];
        udpSocket.readDatagram(data, datagram_size,
                &datagram_source, &datagram_port);

        qDebug() << "PACKET FROM: " << datagram_source;

        if (strncmp(data, TOOL_ACCEPT_MSG, TOOL_ACCEPT_LEN) == 0) {
            RemoteRobot newRemoteRobot(datagram_source,
                    string(data + TOOL_ACCEPT_NAME_OFFSET));
            addRemoteRobot(newRemoteRobot);
        }
    }
    emit refreshedRemoteRobotList();
}

void RobotFinder::addRemoteRobot(RemoteRobot& remoteRobot) {
    remoteRobots.push_back(remoteRobot);
}

}
}
