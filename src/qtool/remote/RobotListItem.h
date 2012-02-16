
#pragma once

#include <QListWidgetItem>
#include "RemoteRobot.h"

namespace qtool {
namespace remote {

class RobotListItem : public QListWidgetItem {

public:
    RobotListItem(RemoteRobot remoteRobot) :
        QListWidgetItem(remoteRobot.getName().c_str()),
        remoteRobot(remoteRobot) {

    }

    const RemoteRobot* getRemoteRobot() const { return &remoteRobot; }

private:
    RemoteRobot remoteRobot;

};

}
}
