#include <QtGui/QApplication>
#include "WorldviewTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::WorldviewTool theTool;
    theTool.show();
    return a.exec();
}
