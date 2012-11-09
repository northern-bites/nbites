#include <QApplication>
#include "VSSPTool.h"

int main(int argv, char **args)
{
    QApplication a(argv, args);
    VSSPTool vssp;
    vssp.show();
    return a.exec();
}
