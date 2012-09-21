/*
 * Main qt application
 */

#include <QApplication>
#include "gui/MainView.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainView mv;
    mv.show();
    return a.exec();
}
