#include <QtGui/QApplication>
#include "SimTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::SimulationTool theTool;
    theTool.show();
    return a.exec();
}
