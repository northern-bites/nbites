
#include "OverseerClient.h"

#include <QVBoxLayout>
#include <QtNetwork/QHostInfo>
#include <iostream>
#include "io/SocketInProvider.h"

namespace qtool {
namespace overseer {

using namespace common::io;
using namespace nbites::overseer;
using namespace std;

OverseerClient::OverseerClient(QWidget* parent) :
        QWidget(parent),
        groundTruth(new GroundTruth()),
        groundTruthView(groundTruth->getProtoMessage()),
        connectButton(new QPushButton("Connect", this)){

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(connectButton);
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectToOverseer()));
    layout->addWidget(&groundTruthView);

    QPushButton* refreshButton = new QPushButton("refresh", this);
    connect(refreshButton, SIGNAL(clicked()), &groundTruthView, SLOT(updateView()));
    layout->addWidget(refreshButton);

    this->setLayout(layout);
}

#include <iostream>

void OverseerClient::connectToOverseer() {

    QHostInfo host_info = QHostInfo::fromName(QString(OVERSEER_HOST.c_str()));

    if (host_info.error() != QHostInfo::NoError) {
        cout << "Could not connect to " << OVERSEER_HOST
             << " : " << host_info.errorString().toStdString() << endl;
        return;
    }

    SocketInProvider::ptr socket_in(new SocketInProvider(
            host_info.addresses().first().toIPv4Address(), OVERSEER_PORT));
    messageParser = MessageParser::ptr(new MessageParser(socket_in, groundTruth));
    messageParser->start();
}

}
}
