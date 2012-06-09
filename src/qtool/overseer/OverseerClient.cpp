
#include "OverseerClient.h"

#include <QVBoxLayout>
#include <QtNetwork/QHostInfo>
#include <iostream>
#include "io/SocketInProvider.h"
#include "Common.h"

namespace qtool {
namespace overseer {

using namespace common::io;
using namespace nbites::overseer;
using namespace data;
using namespace std;
using namespace viewer;
using namespace image;

OverseerClient::OverseerClient(DataManager::ptr dataManager, QWidget* parent) :
        QWidget(parent),
        dataManager(dataManager),
        groundTruth(dataManager->getMemory()->get<GroundTruth>()),
        connectButton(new QPushButton("Connect", this)){

    dataManager->connectSlot(this, SLOT(newGroundTruth()), "GroundTruth");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // field and ground truth painter
    PaintField* fieldImage = new PaintField(this);
    PaintGroundTruth* groundImage = new PaintGroundTruth(groundTruth, this);
    OverlayedImage* combinedImage = new OverlayedImage(fieldImage, groundImage, this);
    viewer::BMPImageViewer* fieldView = new BMPImageViewer(combinedImage, this);
    dataManager->connect(fieldView, SLOT(updateView()), "GroundTruth");
    mainLayout->addWidget(fieldView);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(connectButton);
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectToOverseer()));

    MObjectViewer* groundTruthView = new MObjectViewer(groundTruth->getProtoMessage(), this);
    rightLayout->addWidget(groundTruthView);
    dataManager->connect(groundTruthView, SLOT(updateView()), "GroundTruth");

    QLabel* fpsTagLabel = new QLabel("FPS:", this);
    fpsLabel = new QLabel(this);
    rightLayout->addWidget(fpsTagLabel);
    rightLayout->addWidget(fpsLabel);

    mainLayout->addLayout(rightLayout);
    this->setLayout(mainLayout);
}

// naive fps computation based on delay between two messages
void OverseerClient::newGroundTruth() {
    uint64_t new_timestamp =  groundTruth->get()->timestamp();
    float delta_t = (new_timestamp - last_timestamp) / (float) MICROS_PER_SECOND;
    float fps = 0.0f;
    if (delta_t != 0) {
        fps = 1 / delta_t;
    }

//    float ball_x = groundTruth->get()->ball().x();
//    float ball_y = groundTruth->get()->ball().y();
//
//    if (groundTruth->get()->robots_size() > 0) {
//        auto robot_x = groundTruth->get()->robots(0).x();
//        auto robot_y = groundTruth->get()->robots(0).y();
//
//        cout << "ball distance: "
//             << NBMath::getHypotenuse(ball_x - robot_x, ball_y - robot_y) << endl;
//    }

    fpsLabel->setNum(fps);
    last_timestamp = new_timestamp;
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
    dataManager->newInputProvider(socket_in, "GroundTruth");
}

}
}
