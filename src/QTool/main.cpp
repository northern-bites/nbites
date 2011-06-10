#include <QtGui/QApplication>
#include "QTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTool::QTool qTool;
    qTool.show();
    return a.exec();
}
