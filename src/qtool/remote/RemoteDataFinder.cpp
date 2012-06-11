#include "RemoteDataFinder.h"

#include <iostream>
#include <QHBoxLayout>
#include <QtNetwork/QAbstractSocket>
#include <QLineEdit>

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

    QVBoxLayout* layout = new QVBoxLayout(this);
	QLayout* textBoxLayout = new QHBoxLayout();

	textBox = new QLineEdit(this);
	textBox->setText("Enter dotted IPv4 address");
	textBoxLayout -> addWidget(textBox);

	QPushButton* button = new QPushButton("Connect", this);
	textBoxLayout->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(manualConnect()));

	layout->addLayout(textBoxLayout);

    layout->addWidget(&robotSelect);
    connect(&robotSelect, SIGNAL(robotSelected(const RemoteRobot*)),
            this, SLOT(robotSelected(const RemoteRobot*)));

    for (int i = 0; i < numStreamableObjects; i++) {

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

void RemoteDataFinder::manualConnect() {
	QString addrStr = textBox->text();
	QHostAddress addr(addrStr);

	if (QAbstractSocket::IPv4Protocol != addr.protocol())
	{
		textBox->setText("Invalid IP");
		return;
	}

	RemoteRobot robot(addr, addrStr.toStdString());
	robotSelected(&robot);
}

}
}
