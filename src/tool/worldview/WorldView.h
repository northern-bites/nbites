#pragma once

#include <QtGui>

#include "RoboGrams.h"
#include "WorldModel.pb.h"


namespace tool {
namespace worldview {

class WorldView : public QWidget, public portals::Module{

	Q_OBJECT;

public:
    WorldView(QWidget* parent = 0);

    portals::InPortal<messages::WorldModel> commIn;

protected:
    virtual void run_();
};

}
}
