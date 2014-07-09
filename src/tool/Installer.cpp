#include "Installer.h"

namespace tool{

#define PARAMETERS "../../build/man/install/lib/parameters.json"
#define PY_SWITCH "../../build/man/install/lib/python/players/Switch.py"
#define UPLOAD "../../build/man/upload.sh"

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

    if(!validateInput(index))
    {
        std::cout<<"Did not install." << std::endl;
        return;
    }

    writePlayerNums(pnum.toInt(),team.toInt());
    writePyPlayer(player.toStdString());
    writeAddress(host.toStdString());

    int result = system(UPLOAD);
    std::cout << "Upload exited, returning: " << result << std::endl;
}

void Installer::writePlayerNums(int player, int team)
{
    std::string file = PARAMETERS;
    remove(file.c_str());
    std::ofstream fileOut((file).c_str(), std::ofstream::out);

    fileOut << "{" << std::endl;
    fileOut << "        \"playerNumber\": " <<  player << "," << std::endl;
    fileOut << "        \"teamNumber\": " <<  team << std::endl;
    fileOut << "}" << std::endl;
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
}

int Installer::validateInput(int index)
{
    if(teamNumber->text().toInt() <= 0)
    {
        std::cout << "Bad teamnumber" << std::endl;
        return 0;
    }

    int playerNumber = playerNums.at(index)->text().toInt();
    if(playerNumber < 0 || playerNumber > 6)
    {
        std::cout << "Bad player number" << std::endl;
        return 0;
    }

    std::string requestedPlayer = pyPlayers.at(index)->text().toStdString();
    int size = requestedPlayer.size();

    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    dp = opendir("../../src/man/behaviors/players/");

    bool result = false;
    while((dirp = readdir(dp)) != NULL)
    {
        // This fails if the given pyPlayer is a valid player with some
        // characters trimmed.
        // TODO: fix
        if(!requestedPlayer.compare(std::string(dirp->d_name, size)))
        {
            result = true;
        }
    }
    closedir(dp);
    if(!result)
    {
        std::cout << "Bad python player" << std::endl;
        return 0;
    }

    if(!requestedPlayer.compare("pGoalie"))
    {
        if(playerNumber != 1){
            std::cout << "pGoalie MUST be player 1!" << std::endl;
            return 0;
        }
    }
    else if(!requestedPlayer.compare("pBrunswick"))
    {
        if(playerNumber == 1){
            std::cout << "pBrunswick cannot be player 1!" << std::endl;
            return 0;
        }
    }
    return 1;
}

}
