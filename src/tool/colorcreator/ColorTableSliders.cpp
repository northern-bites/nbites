/**
 * @class ColorTableSliders
 */

#include "ColorTableSliders.h"
#include <QMouseEvent>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QIODevice>

namespace tool {
namespace color {

ColorTableSliders::ColorTableSliders(QWidget *parent) :
    QWidget(parent),
    currentCamera(Camera::TOP),
    topConverter(Camera::TOP),
    bottomConverter(Camera::BOTTOM),
    topDisplay(this),
    bottomDisplay(this),
    topThrDisplay(this),
    botThrDisplay(this),
    bottomImage(base()),
    topImage(base())
{
    subdiagram.addModule(topConverter);
    subdiagram.addModule(bottomConverter);
    subdiagram.addModule(topDisplay);
    subdiagram.addModule(bottomDisplay);
    subdiagram.addModule(topThrDisplay);
    subdiagram.addModule(botThrDisplay);

    topConverter.imageIn.wireTo(&topImage, true);
    bottomConverter.imageIn.wireTo(&bottomImage, true);
    topDisplay.imageIn.wireTo(&topImage, true);
    bottomDisplay.imageIn.wireTo(&bottomImage, true);
    topThrDisplay.imageIn.wireTo(&topConverter.thrImage);
    botThrDisplay.imageIn.wireTo(&bottomConverter.thrImage);

    // GUI
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QVBoxLayout* readWriteButtons = new QVBoxLayout;
    QHBoxLayout* rightInfoLayout = new QHBoxLayout;
    QHBoxLayout* bottomLayout = new QHBoxLayout;


    imageTabs = new QTabWidget(this);
    mainLayout->addWidget(imageTabs);

    imageTabs->addTab(&topDisplay, "Top Image");
    imageTabs->addTab(&bottomDisplay, "Bottom Image");
    imageTabs->addTab(&topThrDisplay, "Top Image Thresh");
    imageTabs->addTab(&botThrDisplay, "Bottom Image Thresh");

    /** Useful information on the color table **/
    colorTableName = new QLabel(this);
    colorTableName->setMaximumHeight(colorTableName->sizeHint().height());
    colorStats = new QLabel(this);
    colorStats->setMaximumHeight(colorTableName->sizeHint().height());

    rightInfoLayout->addWidget(colorTableName);
    rightInfoLayout->addWidget(colorStats);

    /** Load and Save Button Backend **/
    loadColorTableBtn.setText("Load Color Table");
    readWriteButtons->addWidget(&loadColorTableBtn);
    connect(&loadColorTableBtn, SIGNAL(clicked()),
            this, SLOT(loadColorTable()));

    saveColorTableBtn.setText("Save Color Table");
    readWriteButtons->addWidget(&saveColorTableBtn);
    connect(&saveColorTableBtn, SIGNAL(clicked()),
            this, SLOT(saveColorTable()));

    bottomLayout->addLayout(readWriteButtons);

    /** Final layout design **/
    rightInfoLayout->addLayout(bottomLayout);
    mainLayout->addLayout(rightInfoLayout);
    mainLayout->addLayout(bottomLayout);


    setLayout(mainLayout);
}

// This gets called every time the logs are advanced, ie every time the
// "forward" button is pressed in the main tool
void ColorTableSliders::run_()
{
    bottomImageIn.latch();
    topImageIn.latch();

    bottomImage.setMessage(portals::Message<messages::YUVImage>(
                               &bottomImageIn.message()));
    topImage.setMessage(portals::Message<messages::YUVImage>(
                            &topImageIn.message()));

    subdiagram.run();
}

/** Load and Save Color Tables **/
void ColorTableSliders::loadColorTable()
{
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    colorTable.read(filename.toStdString());

    // housekeeping
    colorTableName->setText(filename);
    updateThresholdedImages();
}

void ColorTableSliders::saveColorTable()
{
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getSaveFileName(this,
                    tr("Save Color Table to File"),
                    base_directory + "/new_table.mtb",
                    tr("Color Table files (*.mtb)"));
    colorTable.write(filename.toStdString());
}

void ColorTableSliders::updateColorSelection(int color)
{
    currentColor = color;
}

void ColorTableSliders::updateThresholdedImages()
{
    topConverter.initTable(colorTable.getTable());
    bottomConverter.initTable(colorTable.getTable());

    subdiagram.run();
}

} // namespace color
} // namespace tool
