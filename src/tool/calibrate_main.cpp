#include <QtGui/QApplication>
#include "CalibrationTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::CalibrationTool theTool;
    theTool.show();
    return a.exec();
}
