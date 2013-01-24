/**
 * @class EmptyQTool
 * The main window of QTool without any of its modules.
 * Inherits from qmainwindow. Can be used to create versions of QTool
 * with any subset of modules.
 * Has a data manager and the data handling toolbar.
 */

#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QToolBar>
#include <QScrollArea>
#include <QResizeEvent>

namespace tool {

class EmptyQTool : public QMainWindow {
    Q_OBJECT;

public:
    EmptyQTool(const char* title = "qTool");
    ~EmptyQTool();

protected slots:
    void next();
    void prev();
    void record();

protected:
    virtual void keyPressEvent(QKeyEvent * event);
	void resizeEvent(QResizeEvent*);

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
