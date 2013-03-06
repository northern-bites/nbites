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

#include "ProtoViewer.h"

namespace tool {

namespace unlog
{
class UnlogBase;
}

namespace viewer {

// Things for mapping names to types
typedef tool::unlog::UnlogBase*(* Construct)(std::string);
typedef std::map<std::string, Construct> TypeMap;

class LogViewer : public QMainWindow {
    Q_OBJECT;

public:
    LogViewer(TypeMap map, QWidget* parent = 0);

private:
    std::vector<QDockWidget*> dockWidget;
};

}
}
