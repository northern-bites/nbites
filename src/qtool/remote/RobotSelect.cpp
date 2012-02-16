
#include "RobotSelect.h"
#include "RobotListItem.h"

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
    robotFinder.refresh();

    connect(&robotList, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(robotClicked(QListWidgetItem*)));

    this->setLayout(layout);

}

void RobotSelect::updateList() {
    robotList.clear();
    RemoteRobot::list remoteRobots = robotFinder.getRemoteRobots();
    for (RemoteRobot::list::iterator i = remoteRobots.begin();
                                     i != remoteRobots.end(); i++) {
        robotList.addItem(new RobotListItem(*i));
    }
}

void RobotSelect::robotClicked(QListWidgetItem* item) {
    RobotListItem* clickedItem = dynamic_cast<RobotListItem*>(item);
    emit(robotSelected(clickedItem->getRemoteRobot()));
}

}
}
