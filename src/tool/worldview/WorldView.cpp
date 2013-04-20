#include "WorldView.h"
#include <iostream>
#include <string>

namespace tool {
namespace worldview {


//TODO this is kind of royally screwed
//SET_POOL_SIZE(messages::WorldModel, 24);

WorldView::WorldView(QWidget* parent)
    : commThread("comm", COMM_FRAME_LENGTH_uS),
	  wviewComm(16,69), //TODO for some reason MY_TEAM_NUMBER doesn't work
	  portals::Module(),
	  QWidget(parent)

{
	commThread.addModule(*this);
	commThread.addModule(wviewComm);
	commThread.start();

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

	for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        commIn[i].wireTo(wviewComm._worldModels[i], true);
    }

}


void WorldView::run_()
{
	//this would work if the pool size wasn't an issue
    /*for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        commIn[i].latch();
	    std::cout<<"commPacket says: "<<commIn[i].message().DebugString()<<std::endl;
    }*/

	//proof of concept: latch just one portal
	commIn[3].latch();
	fieldPainter->updateWithLocationMessage(commIn[3].message());
}

}
}
