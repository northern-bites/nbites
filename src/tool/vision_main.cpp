#include <QtGui/QApplication>
#include "VisionTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::VisionTool theTool;
    theTool.show();
    return a.exec();
}
