#include "PlaybookTool.h"

#include <QSize>

namespace tool {

// This file saves the dimensions from the last use of the tool
QFile file(QString("./.geometry"));

PlaybookTool::PlaybookTool(const char* title) :
    playbookCreator(this)
{
    this->setWindowTitle(tr(title));

    this->setCentralWidget(&playbookCreator);


    // Figure out the appropriate dimensions for the window
    if (file.open(QIODevice::ReadWrite)){
        QTextStream in(&file);
        geometry = new QRect(in.readLine().toInt(), in.readLine().toInt(),
                             in.readLine().toInt(), in.readLine().toInt());
        file.close();
    }
    // If we don't have dimensions, default to hard-coded values
    if((geometry->width() == 0) && (geometry->height() == 0)){
        geometry = new QRect(75, 75, 1132, 958);
    }
    this->setGeometry(*geometry);
}

PlaybookTool::~PlaybookTool()
{
    // Write the current dimensions to the .geometry file for next use
    if (file.open(QIODevice::ReadWrite)){
        QTextStream out(&file);
        out << this->pos().x() << "\n"
            << this->pos().y() << "\n"
            << this->width() << "\n"
            << this->height() << "\n";
    }
}

}
