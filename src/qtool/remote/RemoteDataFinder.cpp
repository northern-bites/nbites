#include "RemoteDataFinder.h"

#include <iostream>
#include <QHBoxLayout>

#include "memory/MemoryCommon.h"
#include "CommDef.h"
#include "io/SocketInProvider.h"

namespace qtool {
namespace remote {

using namespace common::io;
using namespace man::memory;

RemoteDataFinder::RemoteDataFinder(QWidget* parent) :
    DataFinder(parent) {

    QLayout* layout = new QHBoxLayout;

    layout->addWidget(&robotSelect);
    connect(&robotSelect, SIGNAL(robotSelected(const RemoteRobot*)),
            this, SLOT(robotSelected(const RemoteRobot*)));

    this->setLayout(layout);
}


void RemoteDataFinder::robotSelected(const RemoteRobot* remoteRobot) {

    for (MObject_ID id = FIRST_OBJECT_ID; id != LAST_OBJECT_ID; id++) {
        InProvider::ptr socket_in = InProvider::ptr(new SocketInProvider(
                remoteRobot->getAddress().toIPv4Address(),
                STREAMING_PORT_BASE + static_cast<short>(id)));
//        socket_in->openCommunicationChannel();
        emit signalNewInputProvider(socket_in);
    }
}

}
}
