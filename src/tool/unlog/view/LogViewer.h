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

#include "ProtoViewer.h"
#include "/home/ecat/nbites/src/tool/unlog/UnlogModule.h"

namespace tool {
namespace unlog {
namespace view {

class LogViewer : public QMainWindow {
    Q_OBJECT;

public:
    LogViewer(QWidget* parent = 0);

    void addProtoViewer(UnlogBase* unlogger);

private:
    std::vector<QDockWidget*> dockWidget;
};

}
}
}
