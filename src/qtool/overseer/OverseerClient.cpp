
#include "OverseerClient.h"

#include <QVBoxLayout>

#include "io/SocketInProvider.h"

namespace qtool {
namespace overseer {

using namespace common::io;
using namespace nbites::overseer;

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

void OverseerClient::connectToOverseer() {

    long ip = getIPForHost(OVERSEER_HOST);

    SocketInProvider::ptr socket_in(new SocketInProvider(ip, OVERSEER_PORT));
    messageParser = MessageParser::ptr(new MessageParser(socket_in, groundTruth));

}

}
}
