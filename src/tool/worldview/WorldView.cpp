#include "WorldView.h"
#include <iostream>
#include <string>

namespace tool {
namespace worldview {

WorldView::WorldView(QWidget* parent)
    : wview_comm(16,0),
	  portals::Module(),
	  QWidget(parent)

{
	commGram.addModule(wview_comm);

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
        commIn[i].wireTo(wview_comm._worldModels[i], true);
    }

}


void WorldView::run_()
{
	for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
		commIn[i].latch();
		std::cout<<"commPacket says: "<<commIn[i].message().DebugString()<<std::endl;
	}
}

}
}
