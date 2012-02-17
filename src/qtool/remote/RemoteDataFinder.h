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
#include "RobotSelect.h"
#include "RemoteRobot.h"

namespace qtool {
namespace remote {

class RemoteDataFinder : public data::DataFinder {
    Q_OBJECT

public:
    RemoteDataFinder(QWidget *parent = 0);
    virtual ~RemoteDataFinder() {}

private:

private slots:
    void robotSelected(const RemoteRobot* remoteRobot);

private:
    RobotSelect robotSelect;
};

}
}
