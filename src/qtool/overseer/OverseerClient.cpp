
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
using namespace viewer;
using namespace image;

OverseerClient::OverseerClient(QWidget* parent) :
        QWidget(parent),
        groundTruth(new GroundTruth()),
        connectButton(new QPushButton("Connect", this)){

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // field and ground truth painter
    PaintField* fieldImage = new PaintField(this);
    PaintGroundTruth* groundImage = new PaintGroundTruth(groundTruth, this);
    OverlayedImage* combinedImage = new OverlayedImage(fieldImage, groundImage, this);
    viewer::BMPImageViewer* fieldView = new BMPImageViewer(combinedImage, this);
    connect(groundTruth.get(), SIGNAL(dataUpdated()),
            fieldView, SLOT(updateView()));
    mainLayout->addWidget(fieldView);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(connectButton);
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectToOverseer()));

    MObjectViewer* groundTruthView = new MObjectViewer(groundTruth->getProtoMessage(), this);
    rightLayout->addWidget(groundTruthView);
    connect(groundTruth.get(), SIGNAL(dataUpdated()),
            groundTruthView, SLOT(updateView()));

    mainLayout->addLayout(rightLayout);
    this->setLayout(mainLayout);
}

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
