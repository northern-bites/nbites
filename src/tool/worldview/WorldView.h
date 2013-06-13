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
    QLabel* subroleLabels[NUM_PLAYERS_PER_TEAM];

    man::DiagramThread commThread;
    man::comm::CommModule wviewComm;

    int newTeam;

protected slots:
    void startButtonClicked();
    void stopButtonClicked();
    void teamChanged();
};

static const QString roles[] = {QString("INIT_ROLE"),
                                QString("PENALTY_ROLE"),
                                QString("CHASER"),
                                QString("MIDDIE"),
                                QString("OFFENDER"),
                                QString("DEFENDER"),
                                QString("GOALIE"),
                                QString("DEFENDER_DUB_D")};

static const QString subroles[] = {QString("INIT_SUB_ROLE"),
                                   QString("PENALTY_SUB_ROLE"),

                                   QString("LEFT_WING"),
                                   QString("RIGHT_WING"),
                                   QString("STRIKER"),
                                   QString("FORWARD"),
                                   QString("PICKER"),

                                   QString("DEFENSIVE_MIDDIE"),
                                   QString("OFFENSIVE_MIDDIE"),
                                   QString("DUB_D_MIDDIE"),

                                   QString("STOPPER"),
                                   QString("SWEEPER"),
                                   QString("CENTER_BACK"),
                                   QString("LEFT_DEEP_BACK"),
                                   QString("RIGHT_DEEP_BACK"),

                                   QString("CHASE_NORMAL"),

                                   QString("GOALIE_CENTER"),
                                   QString("GOALIE_SAVE"),
                                   QString("GOALIE_CHASER"),
                                   QString("GOALIE_KICKOFF"),

                                   QString("KICKOFF_SWEEPER"),
                                   QString("KICKOFF_STRIKER"),

                                   QString("READY_GOALIE"),
                                   QString("READY_CHASER"),
                                   QString("READY_DEFENDER"),
                                   QString("READY_MIDDIE"),
                                   QString("READY_OFFENDER")};

}
}
