#include "Installer.h"

namespace tool{

#define ROBOT_CONFIG "../man/test.h"
#define PY_SWITCH "../man/behaviors/players/test.py"

Installer::Installer(const char* title):
    teamNumber(new QLineEdit(this)),
    playerNames(new QLineEdit(this)),
    pyPlayers(new QLineEdit(this)),
    playerNum(new QLineEdit(this)),
    install(new QPushButton(tr("Install")))
{
    this->setWindowTitle(tr(title));

    QHBoxLayout *player = new QHBoxLayout(this);

    player->addWidget(teamNumber);
    player->addWidget(playerNames);
    player->addWidget(pyPlayers);
    player->addWidget(playerNum);
    player->addWidget(install);

    connect(install, SIGNAL(clicked()), this, SLOT(installPlayer()));
}

void Installer::installPlayer()
{
    QString team = teamNumber->text();
    QString number = playerNum->text();
    QString host = playerNames->text();
    QString player = pyPlayers->text();
    std::vector<std::string> searchFor;

    searchFor.push_back(team.toStdString());
    searchFor.push_back(number.toStdString());

    if(!validateInput())
    {
        std::cout<<"you done fucked up!" << std::endl;
        //return;
    }

    std::fstream* line = new std::fstream("test.h",
                                          std::fstream::in | std::fstream::out);
    std::vector<std::string> list;
    //list.push_back("TEAM");
    list.push_back("PLAYER");
    //std::fstream* f = findLine(line, std::string("he"));
    //for(int i=0; i<list.size(); i++) std::cout << list.at(i) << std::endl;
    //int result = findLine(line, "PLAYER");
    writePlayerNums(55,62);
    writePyPlayer("pBruk2");
    //for(int i=0; i<list.size(); i++) std::cout << list.at(i) << std::endl;
    //f->write(temp.c_str(),1);
    // TODO delete these
}

void Installer::writePlayerNums(int player, int team)
{
    std::string file = ROBOT_CONFIG;
    std::ifstream fileIn(file.c_str(), std::ifstream::in);
    std::ofstream fileOut((file+".tmp").c_str(), std::ofstream::out);
    std::string temp;

    while(getline(fileIn, temp))
    {
        if(-1 != temp.find("PLAYER"))
        {
            fileOut << "#define MY_PLAYER_NUMBER " << player << std::endl;
        }
        else if(-1 != temp.find("TEAM"))
        {
            fileOut << "#define MY_TEAM_NUMBER " << team << std::endl;
        }
        else
        {
            fileOut << temp << std::endl;
        }
    }
    remove(file.c_str());
    rename((file + ".tmp").c_str(), file.c_str());
    remove((file + ".tmp").c_str());
}

void Installer::writePyPlayer(std::string pyPlayer)
{
    std::string file = PY_SWITCH;
    std::ofstream fileOut(file.c_str(), std::ofstream::out);
    fileOut << std::endl << "from . import " << pyPlayer
            << " as selectedPlayer" << std::endl;
}

int Installer::validateInput()
{
    if(!teamNumber->text().toInt()) return 0;
    if(!playerNum->text().toInt()) return 0;
    //TODO finish this;
    else return 1;
}

}
