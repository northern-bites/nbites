#include "Installer.h"

namespace tool{

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
    std::cout << team.toStdString()<<" "<<number.toStdString()<<" "<<host.toStdString()<<" "<<player.toStdString()<<std::endl;
}
}
