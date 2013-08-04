/**
 * @class RobotFinder
 *
 * Will broadcast a UDP request that any online robots will reply to
 * Maintains a list of replying robots
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <vector>
#include <QObject>
#include <QtNetwork/QUdpSocket>

#include "RemoteRobot.h"

namespace qtool {
namespace remote {

class RobotFinder : public QObject {

    Q_OBJECT

public:
    RobotFinder();
    ~RobotFinder() {}

    void addRemoteRobot(RemoteRobot& robot);

    RemoteRobot::list getRemoteRobots() { return remoteRobots; }

public slots:
    void refresh();
    void checkForMessages();

signals:
    void refreshedRemoteRobotList();

protected:
    RemoteRobot::list remoteRobots;

private:
    QUdpSocket udpSocket;
};

}
}
