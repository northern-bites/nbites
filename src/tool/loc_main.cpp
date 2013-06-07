#include <QtGui/QApplication>
#include "LocTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::LocTool theTool;
    theTool.show();
    return a.exec();
}
