#include "WorldView.h"
#include <iostream>
#include <string>

namespace tool {
namespace worldview {


//TODO this is kind of royally screwed
//SET_POOL_SIZE(messages::WorldModel, 24);

WorldView::WorldView(QWidget* parent)
    : commThread("comm", COMM_FRAME_LENGTH_uS),
      wviewComm(16,0),
      portals::Module(),
      QWidget(parent)

{
    commThread.addModule(*this);
    commThread.addModule(wviewComm);

    fieldPainter = new WorldViewPainter(this);
    mainLayout = new QHBoxLayout(this);

    field = new QHBoxLayout();
    field->addWidget(fieldPainter);

    options = new QVBoxLayout();
    options->setAlignment(Qt::AlignTop);
    startButton = new QPushButton(QString("Start World Viewer"));
    options->addWidget(startButton);

    mainLayout->addLayout(field);
    mainLayout->addLayout(options);

    this->setLayout(mainLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));

    // for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    // {
    //     commIn[i].wireTo(wviewComm._worldModels[i]);
    // }

    commIn.wireTo(wviewComm._worldModels[1]);
}


void WorldView::run_()
{
    //this only works when the pool size is increased by ~4x (24 works)
    // for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    // {
    //     commIn[i].latch();
    //  if(!(commIn[i].message().my_x()==0 && commIn[i].message().my_y()==0))
    //      fieldPainter->updateWithLocationMessage(commIn[i].message());
    // }

    //proof of concept: latch just one portal
    commIn.latch();
    fieldPainter->updateWithLocationMessage(commIn.message());
}

void WorldView::startButtonClicked(){
    commThread.start();
    startButton->setText(QString("Stop World Viewer"));
    disconnect(startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(stopButtonClicked()));
}

void WorldView::stopButtonClicked(){
    commThread.stop();
    startButton->setText(QString("Start World Viewer"));
    disconnect(startButton, SIGNAL(clicked()), this, SLOT(stopButtonClicked()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
}

}
}
