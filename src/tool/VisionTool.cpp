#include "VisionTool.h"
#include <QTextStream>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>

namespace tool {

VisionTool::VisionTool(const char* title) :
    tableCreator(this),
    visDispMod(this),
    topConverter(Camera::TOP),
    bottomConverter(Camera::BOTTOM),
    EmptyTool(title)
{
    toolTabs->addTab(&tableCreator, tr("Color Creator"));
    toolTabs->addTab(&visDispMod, tr("Offline Vision"));

    connect(&diagram, SIGNAL(signalUnloggersReady()),
            this, SLOT(setUpModules()));

    QToolBar* toolBar = new QToolBar(this);
    QPushButton* loadBtn = new QPushButton("Load Table", this);
    connect(loadBtn, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    toolBar->addWidget(loadBtn);
    this->addToolBar(toolBar);
}

VisionTool::~VisionTool() {
}

void VisionTool::setUpModules()
{
    diagram.connectToUnlogger<messages::YUVImage>(topConverter.imageIn, "top");
    diagram.connectToUnlogger<messages::YUVImage>(bottomConverter.imageIn, "bottom");

    diagram.addModule(topConverter);
    diagram.addModule(bottomConverter);
    topConverter.initTable(globalColorTable.getTable());
    bottomConverter.initTable(globalColorTable.getTable());

    diagram.addModule(visDispMod);
    diagram.connectToUnlogger<messages::YUVImage>(visDispMod.topImageIn,
                                                  "top");
    diagram.connectToUnlogger<messages::YUVImage>(visDispMod.bottomImageIn,
                                                  "bottom");
    visDispMod.tTImage_in.wireTo(&topConverter.thrImage, true);
    visDispMod.tYImage_in.wireTo(&topConverter.yImage, true);
    visDispMod.tUImage_in.wireTo(&topConverter.uImage, true);
    visDispMod.tVImage_in.wireTo(&topConverter.vImage, true);

    visDispMod.bTImage_in.wireTo(&bottomConverter.thrImage, true);
    visDispMod.bYImage_in.wireTo(&bottomConverter.yImage, true);
    visDispMod.bUImage_in.wireTo(&bottomConverter.uImage, true);
    visDispMod.bVImage_in.wireTo(&bottomConverter.vImage, true);


    /** Color Table Creator Tab **/
    if (diagram.connectToUnlogger<messages::YUVImage>(tableCreator.topImageIn,
                                                      "top") &&
        diagram.connectToUnlogger<messages::YUVImage>(tableCreator.bottomImageIn,
                                                      "bottom"))
    {
        diagram.addModule(tableCreator);
    }
    else
    {
        std::cout << "Right now you can't use the color table creator without"
                  << " two image logs." << std::endl;
    }
}

void VisionTool::loadColorTable()
{

    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    globalColorTable.read(filename.toStdString());
    topConverter.initTable(globalColorTable.getTable());
    bottomConverter.initTable(globalColorTable.getTable());


}


}
