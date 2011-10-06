/**
 * @class QTool
 * main QTool class
 * inherits from qmainwindow
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <qmainwindow.h>
#include <qtabwidget.h>

#include "data/DataLoader.h"
#include "data/DataManager.h"
#include "colorcreator/ColorCreator.h"
#include "viewer/LogViewer.h"
#include "viewer/BallEKFViewer.h"

namespace qtool {

class QTool : public QMainWindow {

    Q_OBJECT

public:
    QTool();
    ~QTool();

private:
    QTabWidget* toolTabs;

    data::DataManager::ptr dataManager;
    data::DataLoader* dataLoader;
    colorcreator::ColorCreator* colorCreator;
    viewer::LogViewer* logViewer;
    viewer::BallEKFViewer* ballEKFViewer;
};

}
