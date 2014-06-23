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

static const QString roles[] = {QString("GOALIE"),
                                QString("LEFT DEFENDER"),
                                QString("RIGHT DEFENDER"),
                                QString("CHASER #1"),
                                QString("CHASER #2"),
                                QString("DROP-IN PLAYER")};
}
}
