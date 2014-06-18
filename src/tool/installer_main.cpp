#include <QtGui/QApplication>
#include "Installer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tool::Installer installer;
    installer.show();
    return a.exec();
}
