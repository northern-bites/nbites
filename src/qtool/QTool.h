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
#include "viewer/MemoryViewer.h"
#include "offline/OfflineViewer.h"
#include "viewer/BallEKFViewer.h"
#include "viewer/FieldViewer.h"
#include "remote/RobotSelect.h"

namespace qtool {

class QTool : public QMainWindow {

    Q_OBJECT

public:
    QTool();
    ~QTool();

private slots:
    void next();
    void prev();

private:
    QTabWidget* toolTabs;

    data::DataManager::ptr dataManager;
    data::DataLoader* dataLoader;
    colorcreator::ColorCreator* colorCreator;
    viewer::MemoryViewer* memoryViewer;
    offline::OfflineViewer* offlineViewer;
    viewer::BallEKFViewer* ballEKFViewer;
    viewer::FieldViewer* fieldViewer;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QToolBar* toolbar;


};

}
