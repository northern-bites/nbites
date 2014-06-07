#pragma once

//#include "EmptyTool.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QString>
#include <iostream>

//#include ROBOT_CONFIG_FILE
namespace tool{

class Installer : public QWidget {
    Q_OBJECT;
public:
    Installer(const char* title = "The Installer");
protected:
    QLineEdit* teamNumber;
    QLineEdit* playerNames; // TODO: Think about this, NUM_PLAYERS_PER_TEAM?
    QLineEdit* pyPlayers; // TODO
    QLineEdit* playerNum; // TODO
    QPushButton* install; // TODO
protected slots:
    void installPlayer();
};
}
