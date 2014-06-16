#pragma once

//#include "EmptyTool.h"ostream/ostream/write/
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QSignalMapper>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>

//#include ROBOT_CONFIG_FILE
namespace tool{

class Installer : public QWidget {
    Q_OBJECT;
public:
    Installer(const char* title = "The Installer");
protected:
    std::vector<QLineEdit*> playerNames;
    std::vector<QLineEdit*> pyPlayers;
    std::vector<QLineEdit*> playerNums;
    std::vector<QPushButton*> installButtons;
    QLineEdit* teamNumber;
    //QLineEdit* playerNames; // TODO: Think about this, NUM_PLAYERS_PER_TEAM?
    //QLineEdit* pyPlayers; // TODO
    //QLineEdit* playerNum; // TODO
    //QPushButton* install; // TODO
    //std::fstream* findLine(std::fstream* stream, std::string lineStart);
    void writePlayerNums(int player, int team);
    void writePyPlayer(std::string pyPlayer);
    void writeAddress(std::string address);
    int validateInput(int index);
protected slots:
    void installPlayer(int index);
//private:
};
}
