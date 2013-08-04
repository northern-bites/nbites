#include <QtGui/QApplication>
#include "BallTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::BallTool theTool;
    theTool.show();
    return a.exec();
}
