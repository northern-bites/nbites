/**
 * @class OfflineDataFinder
 *
 * Extends DataFinder
 * A widget that lets the user browse his local files for a data source
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <QVector>
#include <QCheckBox>

#include "data/DataFinder.h"
#include "data/DataManager.h"
#include "RobotSelect.h"
#include "RemoteRobot.h"

namespace qtool {
//TODO: this should be moved to data and the data namespace
namespace remote {

class RemoteDataFinder : public data::DataFinder {
    Q_OBJECT

public:
    RemoteDataFinder(data::DataManager::ptr dataManager, QWidget *parent = 0);
    virtual ~RemoteDataFinder() {}

private:

private slots:
    void robotSelected(const RemoteRobot* remoteRobot);
	void manualConnect();
	void disconnect();

private:
    data::DataManager::ptr dataManager;
	QLineEdit* textBox;
    RobotSelect robotSelect;
    QVector<QCheckBox*> objectSelectVector;
};

}
}
