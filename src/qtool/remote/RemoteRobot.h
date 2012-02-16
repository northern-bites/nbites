/**
 * @class RemoteRobot
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <vector>
#include <QtNetwork/QHostAddress>

namespace qtool {
namespace remote {

class RemoteRobot {

public:
    typedef std::vector<RemoteRobot> list;

public:
    RemoteRobot(QHostAddress address, std::string name) :
         address(address), name(name) {}
    RemoteRobot() {}
    ~RemoteRobot() {}

    std::string getName() const {return name;}
    QHostAddress getAddress() const {return address;}

private:
    std::string name;
    QHostAddress address;

};

}
}
