/**
 * @class EmptyQTool
 * The main window of QTool without any of its modules.
 * Inherits from qmainwindow. Can be used to create versions of QTool
 * with any subset of modules.
 * Has a data manager and the data handling toolbar.
 */

#pragma once

#include <qmainwindow.h>
#include <qtabwidget.h>
#include <QPushButton>
#include <QToolBar>
#include <QScrollArea>
#include <QResizeEvent>
#include <QTextStream>

#include "data/DataManager.h"

#ifndef NBITES_DIR
#define NBITES_DIR "~/nbites"
#warning "Could not find NBITES_DIR define! Reverting to " NBITES_DIR
#endif

namespace qtool {

class EmptyQTool : public QMainWindow {

    Q_OBJECT

public:
    EmptyQTool(const char* title = "qTool");
    ~EmptyQTool();

protected slots:
    void next();
    void prev();
    void record();

protected:
    virtual void keyPressEvent(QKeyEvent * event);

    QTabWidget* toolTabs;

    data::DataManager::ptr dataManager;

    QPushButton* prevButton;
    QPushButton* nextButton;
    QPushButton* recordButton;
	QPushButton* scrollButton;
    QToolBar* toolbar;
	QScrollArea* scrollArea;
	QSize* tabStartSize;
	QRect* geom;
	QSize* scrollBarSize;

	void resizeEvent(QResizeEvent*);
};

}
