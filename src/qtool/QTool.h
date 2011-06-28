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

namespace qtool {

class QTool : public QMainWindow {

    Q_OBJECT

public:
    QTool();
    ~QTool();

private:
    QTabWidget* toolTabs;

    data::DataManager* dataManager;
    data::DataLoader* dataLoader;
    colorcreator::ColorCreator* colorCreator;
};

}
