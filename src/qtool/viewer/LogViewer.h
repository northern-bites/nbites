/**
 * @class Viewer
 *
 * A class that maintains a UI that views the data
 * that the data manager has
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <QWidget>
#include <QtGui>

#include "data/DataManager.h"
#include "image/RoboImageViewer.h"
#include "data/treemodel/TreeModel.h"
#include "data/treemodel/ProtoNode.h"

namespace qtool {
namespace viewer {

class LogViewer : public QWidget {
	Q_OBJECT

public:
	LogViewer(data::DataManager::ptr dataManager);

private:
	data::DataManager::ptr dataManager;
	RoboImageViewer* roboImageViewer;

};

}
}
