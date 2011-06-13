
#include "QTool.h"
#include <iostream>

namespace QTool {

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        colorCreator(new ColorCreator()){

    this->setWindowTitle(tr("HackTool"));

    this->setCentralWidget(toolTabs);

    toolTabs->addTab(colorCreator, tr("Color Creator"));
}

QTool::~QTool() {
    delete toolTabs;

    delete colorCreator;
}

}
