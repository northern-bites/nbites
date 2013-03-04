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
#include <QCheckBox>

#include "MObjectViewer.h"

namespace tool {
namespace viewer {

class MemoryViewer : public QMainWindow{
    Q_OBJECT;

public:
    MemoryViewer(QWidget* parent = 0);

private:
    std::vector<QDockWidget*> dockWidget;
};

}
}
