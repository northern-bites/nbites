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
#include "colorcreator/ColorCalibrate.h"
#include "viewer/MemoryViewer.h"
#include "offline/OfflineViewer.h"
#include "viewer/BallEKFViewer.h"
#include "viewer/FieldViewer.h"
#include "remote/RobotSelect.h"

#ifndef NBITES_DIR
#define NBITES_DIR "~/nbites"
#warning "Could not find NBITES_DIR define! Reverting to " NBITES_DIR
#endif


namespace qtool {

class QTool : public QMainWindow {

    Q_OBJECT

public:
    QTool();
    ~QTool();

private slots:
    void next();
    void prev();
    void record();

private:
    QTabWidget* toolTabs;
<<<<<<< HEAD
    QScrollArea *colorScrollArea;
=======
    QScrollArea* colorScrollArea;
>>>>>>> newBots

    data::DataManager::ptr dataManager;
    data::DataLoader* dataLoader;
    colorcreator::ColorCalibrate* colorCalibrate;
    viewer::MemoryViewer* memoryViewer;
    offline::OfflineViewer* offlineViewer;
    viewer::BallEKFViewer* ballEKFViewer;
    viewer::FieldViewer* fieldViewer;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QPushButton* recordButton;
    QToolBar* toolbar;


};

}
