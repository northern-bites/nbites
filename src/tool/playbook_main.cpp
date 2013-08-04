#include <QtGui/QApplication>
#include "PlaybookTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::PlaybookTool theTool;
    theTool.show();
    return a.exec();
}
