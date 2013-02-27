/**
 * @class RobotSelect
 *
 * Provides a GUI interface to select a remote robot and
 * returns a data source based off it
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QListWidget>
#include <QLayout>
#include <QPushButton>
#include "RobotFinder.h"
///#include "io/SocketFDProvider.h"

namespace qtool {
namespace remote {

class RobotSelect : public QWidget {

    Q_OBJECT

public:
    RobotSelect(QWidget* parent = 0);
    ~RobotSelect() {}

signals:
    void robotSelected(const RemoteRobot* remoteRobot);

protected slots:
    void updateList();
    void robotClicked(QListWidgetItem* item);

protected:
    QListWidget robotList;
    RobotFinder robotFinder;
    QListWidgetItem* currentSelection;

};

}
}
