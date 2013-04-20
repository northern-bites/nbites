/**
 * @class RobotFieldViewer
 *
 * A widget with drawings of the robot's mindset on the field
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QWidget>

#include "data/DataManager.h"
#include "image/PaintField.h"
#include "image/PaintRobotWorldState.h"
#include "BMPImageViewer.h"

namespace qtool {
namespace viewer {

class RobotField: public QWidget {

    Q_OBJECT

public:
    RobotField(data::DataManager::ptr dataManager, QWidget* parent = 0);

private:
    data::DataManager::ptr dataManager;
};

}
}
