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
#include "vision_offline/VisionDisplayModule.h"

namespace tool {

class Tool : public QMainWindow {
    Q_OBJECT;

public:
    Tool(const char* title = "TOOL");
    ~Tool();

public slots:
    void setUpModules();

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
	

    // GUI stuff
    QTabWidget* toolTabs;
    QToolBar* toolbar;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QPushButton* recordButton;
    QPushButton* scrollButton;
    QScrollArea* scrollArea;
    QSize* scrollBarSize;
    QSize* tabStartSize;
    QRect* geometry;
};
}
