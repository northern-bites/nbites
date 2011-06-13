#include <QtGui/QApplication>
#include "QTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qtool::QTool qTool;
    qTool.show();
    return a.exec();
}
