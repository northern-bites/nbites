#include <QtGui/QApplication>
#include "Tool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::Tool theTool;
    theTool.show();
    return a.exec();
}
