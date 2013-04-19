#include "WorldView.h"
#include <iostream>
#include <string>

namespace tool {
namespace worldview {

WorldView::WorldView(QWidget* parent)
    : portals::Module(),
	  QWidget(parent)
{
    fieldPainter = new WorldViewPainter(this);
    mainLayout = new QHBoxLayout(this);

	field = new QHBoxLayout();
    field->addWidget(fieldPainter);

	options = new QVBoxLayout();

    mainLayout->addLayout(field);

    this->setLayout(mainLayout);
}


void WorldView::run_()
{
    commIn.latch();
	std::cout<<"commPacket says: "<<commIn.message().DebugString()<<std::endl;
}

}
}
