#include <QtGui/QApplication>
#include "SharedTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::SharedTool theTool;
    theTool.show();
    return a.exec();
}
