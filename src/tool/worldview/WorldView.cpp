#include "WorldView.h"
#include <iostream>
#include <string>

namespace tool {
namespace worldview {

WorldView::WorldView(QWidget* parent)
    : portals::Module(),
	  QWidget(parent)
{
}

void WorldView::run_()
{
    commIn.latch();
    messages::WorldModel commPacket = commIn.message();


}

}
}
