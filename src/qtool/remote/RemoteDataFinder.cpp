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
using namespace data;

RemoteDataFinder::RemoteDataFinder(DataManager::ptr dataManager, QWidget* parent) :
    DataFinder(parent), dataManager(dataManager) {

    QLayout* layout = new QVBoxLayout;

    layout->addWidget(&robotSelect);
    connect(&robotSelect, SIGNAL(robotSelected(const RemoteRobot*)),
            this, SLOT(robotSelected(const RemoteRobot*)));

    for (int i = 0; i < numStreamableObjects; i++) {

        std:: cout << i << std::endl;

        QCheckBox* checkBox = new QCheckBox(QString(streamableObjects[i].c_str()), this);
        checkBox->setChecked(true);

        layout->addWidget(checkBox);
        objectSelectVector.push_back(checkBox);
    }

    this->setLayout(layout);
}

void RemoteDataFinder::robotSelected(const RemoteRobot* remoteRobot) {

    emit signalNewDataSet();

    //TODO: this is a somewhat terrible way of negociating which and how many
    //sockets to use, as the order of the object in the memory map can change
    //an alternative is to actually use a protocol to negociate which object
    //should go on which port

    Memory::const_ptr memory = dataManager->getMemory();
    unsigned short port_offset = 0;

    for (QVector<QCheckBox*>::Iterator it = objectSelectVector.begin();
                                       it != objectSelectVector.end();
                                       it++, port_offset++) {
        if ((*it)->isChecked()) {
            InProvider::ptr socket_in = InProvider::ptr(new SocketInProvider(
                    remoteRobot->getAddress().toIPv4Address(),
                    STREAMING_PORT_BASE + port_offset));
            emit signalNewInputProvider(socket_in, (*it)->text().toStdString());
        }
    }
}

}
}
