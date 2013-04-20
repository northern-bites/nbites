#pragma once

#include <QtGui>

#include "WorldModel.pb.h"

#include "WorldViewPainter.h"

#include "comm/CommModule.h"
#include "RobotConfig.h"
#include "RoboGrams.h"
#include "DiagramThread.h"

namespace tool {
namespace worldview {

class WorldView : public QWidget, public portals::Module{

	Q_OBJECT;

public:
    WorldView(QWidget* parent = 0);

    portals::InPortal<messages::WorldModel> commIn[NUM_PLAYERS_PER_TEAM];

protected:
    virtual void run_();

protected:
	WorldViewPainter* fieldPainter;

    QHBoxLayout* mainLayout;
    QHBoxLayout* field;
    QVBoxLayout* options;

	QPushButton* startButton;

    man::comm::CommModule wviewComm;

    man::DiagramThread commThread;

protected slots:
    void startButtonClicked();
    void stopButtonClicked();
};

}
}
