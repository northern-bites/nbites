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
#include <vector>

#include "data/DataManager.h"
#include "RoboImageViewer.h"
#include "MObjectViewer.h"

namespace qtool {
namespace viewer {

class LogViewer : public QMainWindow{
	Q_OBJECT

public:
	LogViewer(data::DataManager::ptr dataManager);

public slots:
    void nextImage();
    void prevImage();

private:
    std::vector<QDockWidget*> dockWidget;
    data::DataManager::ptr dataManager;
    RoboImageViewer* roboImageViewer;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QToolBar* toolbar;
};

}
}
