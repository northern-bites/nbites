
#include "QTool.h"
#include <iostream>

namespace QTool {

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        colorCreator(new ColorCreator()){

    this->setWindowTitle(tr("QTool - the only tool you need!"));

    this->setCentralWidget(toolTabs);

    toolTabs->addTab(colorCreator, tr("Color Creator"));
}

QTool::~QTool() {
    delete toolTabs;

    delete colorCreator;
}

}
