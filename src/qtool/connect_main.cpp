#include <QtGui/QApplication>
#include "ConnectTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qtool::ConnectTool qTool;
    qTool.show();
    return a.exec();
}
