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

    void updateStatus(messages::WorldModel msg, int playerIndex);

protected:
    WorldViewPainter* fieldPainter;

    QPushButton* startButton;

    QLineEdit* teamSelector;

    QLabel* roleLabels[NUM_PLAYERS_PER_TEAM];

    man::DiagramThread commThread;
    man::comm::CommModule wviewComm;

    int newTeam;
    QMutex mutex;

protected slots:
    void startButtonClicked();
    void stopButtonClicked();
    void teamChanged();
};

static const QString roles[] = {QString("CHASER"),
                                QString("MIDDIE"),
                                QString("OFFENDER"),
                                QString("DEFENDER"),
                                QString("GOALIE"),
                                QString("PENALTY_ROLE"),
                                QString("READY_ROLE"),
                                QString("INIT_ROLE")};

}
}
