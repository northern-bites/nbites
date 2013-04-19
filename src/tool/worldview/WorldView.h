#pragma once

#include <QtGui>

#include "RoboGrams.h"
#include "WorldModel.pb.h"


namespace tool {
namespace worldview {

class WorldView : public portals::Module, public QWidget {

	Q_OBJECT;

public:
    WorldView(QWidget* parent = 0);
    virtual ~WorldView() {}

    portals::InPortal<messages::WorldModel> commIn;

protected:
    virtual void run_();
};

}
}
