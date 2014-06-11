#include "Installer.h"

namespace tool{

#define ROBOT_CONFIG "../man/RobotConfig.h"
#define PY_SWITCH "../man/behaviors/players/test.py"
#define UPLOAD "../man/up.sh"

Installer::Installer(const char* title):
    teamNumber(new QLineEdit("Team Number", this)),
    playerNames(),
    pyPlayers(),
    playerNums(),
    installButtons()

{
    this->setWindowTitle(tr(title));

    QGridLayout *main = new QGridLayout(this);
    main->setSpacing(2);
    main->addWidget(teamNumber, 0, 0);
    main->addWidget(new QLabel("Host"),1,0);
    main->addWidget(new QLabel("Python Player"),1,1);
    main->addWidget(new QLabel("Player Number"),1,2);

    QSignalMapper *signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(installPlayer(int)));


    for(int i=2; i<7; i++){
        QLineEdit* name = new QLineEdit(this);
        QLineEdit* pyPlay = new QLineEdit(this);
        QLineEdit* playNum = new QLineEdit(this);
        QPushButton* install = new QPushButton(tr("Install"));

        signalMapper->setMapping(install, i-2);
        connect(install, SIGNAL(clicked()), signalMapper, SLOT(map()));

        main->addWidget(name, i, 0);
        main->addWidget(pyPlay, i, 1);
        main->addWidget(playNum, i, 2);
        main->addWidget(install, i, 3);

        playerNames.push_back(name);
        pyPlayers.push_back(pyPlay);
        playerNums.push_back(playNum);
        installButtons.push_back(install);
    }
}

void Installer::installPlayer(int index)
{
    QString team = teamNumber->text();
    QString pnum = playerNums.at(index)->text();
    QString host = playerNames.at(index)->text();
    QString player = pyPlayers.at(index)->text();

    // TODO: implement
    if(!validateInput())
    {
        std::cout<<"you done fucked up!" << std::endl;
        //return;
    }

    writePlayerNums(pnum.toInt(),team.toInt());
    writePyPlayer(player.toStdString());
    writeAddress(host.toStdString());
    // TODO delete these

    int result = system(UPLOAD);
    std::cout << result << std::endl;
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

void Installer::writeAddress(std::string address)
{
    std::string file = UPLOAD;
    std::ifstream fileIn(file.c_str(), std::ifstream::in);
    std::string temp;
    std::vector<std::string> fileHolder;

    // The two while loops are necessary to preserve file permissions on
    // upload.sh since it needs to be executable
    while(getline(fileIn, temp))
    {
        fileHolder.push_back(temp);
    }

    std::ofstream fileOut((file).c_str(), std::ofstream::out);
    for(std::vector<std::string>::iterator it = fileHolder.begin(); it != fileHolder.end(); it++)
    {
        if(-1 != it->find("ADDRESS="))
        {
            fileOut << "ADDRESS=" << address << std::endl;
        }
        else
        {
            fileOut << *it << std::endl;
        }
    }
    //remove(file.c_str());
    //rename((file+".tmp").c_str(), file.c_str());
}

int Installer::validateInput()
{
    if(!teamNumber->text().toInt()) return 0;
    //if(!playerNum->text().toInt()) return 0;
    //TODO finish this;
    else return 1;
}

}
