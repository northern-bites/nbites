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

#include "unlog/UnlogModule.h"
#include "image/ImageDisplayModule.h"
#include "ProtoViewer.h"

namespace tool {
namespace logview {

class LogViewer : public QMainWindow {
    Q_OBJECT;

public:
    LogViewer(QWidget* parent = 0);

public slots:
    void newDisplayWidget(QWidget*, std::string);
    void deleteDisplayWidgets();

protected:
    QTabWidget imageTabs;
    std::vector<QDockWidget*> dockWidgets;
};

}
}
