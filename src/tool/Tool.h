/**
 * Lizzie write new comments
 */

#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QToolBar>
#include <QScrollArea>
#include <QResizeEvent>

#include "RoboGrams.h"

namespace tool {

class Tool : public QMainWindow {
    Q_OBJECT;

public:
    Tool(const char* title = "qTool");
    ~Tool();

protected slots:
    void next();
    void prev();
    void record();

protected:
    virtual void keyPressEvent(QKeyEvent * event);
	void resizeEvent(QResizeEvent*);

    portals::RoboGram mainDiagram;

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
