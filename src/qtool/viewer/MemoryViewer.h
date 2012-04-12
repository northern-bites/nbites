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

#include "man/memory/Memory.h"
#include "image/BMPYUVImage.h"

#include "image/OverlayImage.h"

#include "data/RobotMemoryManager.h"
#include "BMPImageViewer.h"
#include "ChannelImageViewer.h"
#include "MObjectViewer.h"

namespace qtool {
namespace viewer {

class MemoryViewer : public QMainWindow{
    Q_OBJECT

public:
    MemoryViewer(data::RobotMemoryManager::const_ptr memoryManager);

private:
    std::vector<QDockWidget*> dockWidget;
    data::RobotMemoryManager::const_ptr memoryManager;
    //viewer::BMPImageViewer imageViewer;
    //    image::Overlaye
};

}
}
