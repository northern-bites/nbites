/**
 * @class OfflineDataFinder
 *
 * Extends DataFinder
 * A widget that lets the user browse his local files for a data source
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

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

private:
    data::DataManager::ptr dataManager;
    RobotSelect robotSelect;
};

}
}
