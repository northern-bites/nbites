/**
 * The main Tool class!
 */

#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QToolBar>
#include <QScrollArea>
#include <QResizeEvent>
#include <QPixmap>
#include <QImage>
#include <QLabel>

#include "ToolDiagram.h"
#include "DataSelector.h"
#include "logview/LogViewer.h"
#include "colorcreator/ColorTableCreator.h"
#include "worldview/WorldView.h"

#include "colorcreator/ColorTable.h"
#include "vision_offline/VisionDisplayModule.h"
#include "fieldview/FieldViewer.h"

#include "image/ImageConverterModule.h"

#include "colorcreator/ColorCalibrate.h"
#include "playbook/PlaybookCreator.h"

#include "ParticleSwarm.pb.h"

namespace tool {

class Tool : public QMainWindow {
    Q_OBJECT;

public:
    Tool(const char* title = "TOOL");
    ~Tool();

public slots:
    void setUpModules();
    void loadColorTable();
    void changeTableValues(std::vector<color::colorChanges> tableAdjustments);
    void unChangeTableValues(std::vector<color::colorChanges> tableAdjustments);
    void saveAsGlobalTable();
    void saveGlobalTable();

private:
    void serializeTableName(QString latestTableName);
    void loadLatestTable();

protected:
    // For keyboard control
    virtual void keyPressEvent(QKeyEvent * event);

    void resizeEvent(QResizeEvent*);

    // Modules in this diagram will be run when data is updated
    ToolDiagram diagram;

    DataSelector selector;
    logview::LogViewer logView;
    color::ColorTableCreator tableCreator;
    vision::VisionDisplayModule visDispMod;
    colorcreator::ColorCalibrate colorCalibrate;
    viewer::FieldViewer fieldView;
    playbook::PlaybookCreator playbookCreator;

    man::image::ImageConverterModule topConverter;
    man::image::ImageConverterModule bottomConverter;

    color::ColorTable globalColorTable;
    worldview::WorldView worldView;


    // GUI stuff
    QTabWidget* toolTabs;
    QToolBar* toolbar;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QPushButton* scrollButton;
    QPushButton* loadBtn;
    QScrollArea* scrollArea;
    QSize* scrollBarSize;
    QSize* tabStartSize;
    QRect* geometry;
};
}
