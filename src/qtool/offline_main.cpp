#include <QtGui/QApplication>
#include "OfflineTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qtool::OfflineTool offlineTool;
    offlineTool.show();
    return a.exec();
}
