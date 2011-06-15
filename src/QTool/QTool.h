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

#include "data/DataManager.h"
#include "colorcreator/colorcreator.h"

namespace qtool {

class QTool : public QMainWindow {

    Q_OBJECT

public:
    QTool();
    ~QTool();

private:
    QTabWidget* toolTabs;

    data::DataManager* dataManager;
    ColorCreator* colorCreator;
};

}
