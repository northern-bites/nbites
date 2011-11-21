
#include "RobotSelect.h"

namespace qtool {
namespace remote {

RobotSelect::RobotSelect(QWidget* parent) :
        QWidget(parent), robotList(this) {

    QVBoxLayout* layout = new QVBoxLayout;

    connect(&robotFinder, SIGNAL(refreshedRemoteRobotList()),
            this, SLOT(updateList()));
    layout->addWidget(&robotList);

    QPushButton* refreshButton = new QPushButton(tr("&Refresh"));
    connect(refreshButton, SIGNAL(clicked()), &robotFinder, SLOT(refresh()));
    layout->addWidget(refreshButton);

    this->setLayout(layout);

}

void RobotSelect::updateList() {
    robotList.clear();
    RemoteRobot::list remoteRobots;
    robotFinder.getRemoteRobots(remoteRobots);
    for (RemoteRobot::list::iterator i = remoteRobots.begin();
                                     i != remoteRobots.end(); i++) {
        robotList.addItem(QString((*i).getName().c_str()));
    }
}

}
}
