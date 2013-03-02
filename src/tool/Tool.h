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

#include "ToolDiagram.h"

namespace tool {

class Tool : public QMainWindow {
    Q_OBJECT;

public:
    Tool(const char* title = "TOOL");
    ~Tool();

protected slots:
    // Called by the buttons in the main toolbar
    void next();
    void prev();
    void record();

protected:
    // For keyboard control
    virtual void keyPressEvent(QKeyEvent * event);

	void resizeEvent(QResizeEvent*);

    // Modules in this diagram will be run when data is updated
    ToolDiagram diagram;

    // Modules

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
