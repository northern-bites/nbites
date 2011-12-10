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
#include "image/TestImage.h"
#include "RoboImageViewer.h"
#include "MObjectViewer.h"

namespace qtool {
namespace viewer {

class MemoryViewer : public QMainWindow{
    Q_OBJECT

public:
    MemoryViewer(man::memory::Memory::const_ptr memory);

private:
    std::vector<QDockWidget*> dockWidget;
    man::memory::Memory::const_ptr memory;
    image::BMPImage::ptr image;
    image::TestImage::ptr overlay;
    RoboImageViewer* roboImageViewer;

};

}
}
