#pragma once

//#include "EmptyTool.h"ostream/ostream/write/
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QString>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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
    //std::fstream* findLine(std::fstream* stream, std::string lineStart);
    void writePlayerNums(int player, int team);
    void writePyPlayer(std::string pyPlayer);
    int validateInput();
protected slots:
    void installPlayer();
//private:
};
}
