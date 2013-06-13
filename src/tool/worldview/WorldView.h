#pragma once

#include <QtGui>

#include "WorldModel.pb.h"

#include "WorldViewPainter.h"

#include "comm/CommModule.h"
#include "Common.h"
#include "RoboGrams.h"
#include "DiagramThread.h"

namespace tool {
namespace worldview {

class WorldView : public QWidget, public portals::Module {

Q_OBJECT;

public:
    WorldView(QWidget* parent = 0);

    portals::InPortal<messages::WorldModel> commIn[NUM_PLAYERS_PER_TEAM];

protected:
    virtual void run_();

protected:
    WorldViewPainter* fieldPainter;

    QPushButton* startButton;

    QLabel* roleLabels[NUM_PLAYERS_PER_TEAM];
    QLabel* subroleLabels[NUM_PLAYERS_PER_TEAM];

    man::DiagramThread commThread;
    man::comm::CommModule wviewComm;

protected slots:
    void startButtonClicked();
    void stopButtonClicked();
};

}
}
