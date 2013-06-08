/**
 * @class ColorTableCreator
 */

#include "ColorTableCreator.h"
#include <QMouseEvent>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QIODevice>

namespace tool {
namespace color {

ColorTableCreator::ColorTableCreator(QWidget *parent) :
    QWidget(parent),
    currentCamera(Camera::TOP),
    topConverter(),
    bottomConverter(),
    topDisplay(this),
    bottomDisplay(this),
    thrDisplay(this),
    bottomImage(base()),
    topImage(base())
{
    // BACKEND
    // We need converter modules to threshold both the top and bottom images,
    // and ImageDisplayModule for each image, and a ThresholdedDisplayModule
    // for whichever image is currently being workied on
    subdiagram.addModule(topConverter);
    subdiagram.addModule(bottomConverter);
    subdiagram.addModule(topDisplay);
    subdiagram.addModule(bottomDisplay);
    subdiagram.addModule(thrDisplay);

    topConverter.imageIn.wireTo(&topImage, true);
    bottomConverter.imageIn.wireTo(&bottomImage, true);
    topDisplay.imageIn.wireTo(&topImage, true);
    bottomDisplay.imageIn.wireTo(&bottomImage, true);
    thrDisplay.imageIn.wireTo(&topConverter.thrImage);

    // GUI
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QHBoxLayout* leftLayout = new QHBoxLayout;

    imageTabs = new QTabWidget(this);
    leftLayout->addWidget(imageTabs);

    imageTabs->addTab(&topDisplay, "Top Image");
    imageTabs->addTab(&bottomDisplay, "Bottom Image");

    connect(imageTabs, SIGNAL(currentChanged(int)),
            this, SLOT(imageTabSwitched(int)));

    connect(&topDisplay, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(canvasClicked(int, int, int, bool)));
    connect(&bottomDisplay, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(canvasClicked(int, int, int, bool)));

    QVBoxLayout* rightLayout = new QVBoxLayout;

    colorTableName = new QLabel(this);
    colorTableName->setMaximumHeight(colorTableName->sizeHint().height());

    colorStats = new QLabel(this);
    colorStats->setMaximumHeight(colorTableName->sizeHint().height());

    //set up the color selection combo box
    for (int i = 0; i < image::Color::NUM_COLORS; i++) {
        colorSelect.addItem(image::Color_label[i].c_str());
    }
    connect(&colorSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateColorSelection(int)));
    colorSelect.setCurrentIndex(STARTING_COLOR);
    rightLayout->addWidget(&colorSelect);

    QPushButton* undoBtn = new QPushButton("Undo", this);
    connect(undoBtn, SIGNAL(clicked()), this, SLOT(undo()));
    rightLayout->addWidget(undoBtn);

    QPushButton* loadBtn = new QPushButton("Load", this);
    connect(loadBtn, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    rightLayout->addWidget(loadBtn);

    QPushButton* saveAsBtn = new QPushButton("Save as", this);
    rightLayout->addWidget(saveAsBtn);
    connect(saveAsBtn, SIGNAL(clicked()), this, SLOT(saveColorTableAs()));

    QPushButton* saveBtn = new QPushButton("Save", this);
    rightLayout->addWidget(saveBtn);
    connect(saveBtn, SIGNAL(clicked()), this, SLOT(saveColorTable()));

    rightLayout->addWidget(&thrDisplay);
    rightLayout->addWidget(colorTableName);
    rightLayout->addWidget(colorStats);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);
    loadLatestTable();
}

// Note: serizalization done by Qt
void ColorTableCreator::loadLatestTable()
{
    if (imageTabs->currentIndex() == 0) {
        QFile file("../../data/tables/latestTopTable.dat");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        QString filename;
        in >> filename;
        colorTable.read(filename.toStdString());
        colorTableName->setText(filename);
    }
    else {
        QFile file("../../data/tables/latestBottomTable.dat");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        QString filename;
        in >> filename;
        colorTable.read(filename.toStdString());
        colorTableName->setText(filename);
    }
    updateColorStats();
}

// Note: serizalization done by Qt
void ColorTableCreator::serializeTableName(QString latestTableName) 
{
    if (imageTabs->currentIndex() == 0) {
        QFile file("../../data/tables/latestTopTable.dat");
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        out << latestTableName;
    }
    else {
        QFile file("../../data/tables/latestBottomTable.dat");
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        out << latestTableName;
    }
}

// This gets called every time the logs are advanced, ie every time the
// "forward" button is pressed in the main tool
void ColorTableCreator::run_()
{
    bottomImageIn.latch();
    topImageIn.latch();

    messages::YUVImage botIn = bottomImageIn.message();
    messages::YUVImage topIn = topImageIn.message();

    std::cout << "Image counter: " << topIn.getFC() << "\n";

    bottomImage.setMessage(portals::Message<messages::YUVImage>(
                               &botIn));
    topImage.setMessage(portals::Message<messages::YUVImage>(
                            &topIn));

    updateThresholdedImage();
}

void ColorTableCreator::loadColorTable()
{
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    colorTable.read(filename.toStdString());
    colorTableName->setText(filename);

    serializeTableName(filename);
    updateThresholdedImage();
}

void ColorTableCreator::saveColorTableAs()
{
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getSaveFileName(this,
                    tr("Save Color Table to File"),
                    base_directory + "/new_table.mtb",
                    tr("Color Table files (*.mtb)"));
    colorTable.write(filename.toStdString());
    colorTableName->setText(filename);

    serializeTableName(filename);
}

void ColorTableCreator::saveColorTable()
{
    QString filename;
    if (imageTabs->currentIndex() == 0) {
        QFile file("../../data/tables/latestTopTable.dat");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        in >> filename;
    }
    else {
        QFile file("../../data/tables/latestBottomTable.dat");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        in >> filename;
    }
    colorTable.write(filename.toStdString());
    colorTableName->setText(filename);
}

// Updates the color tables for both image converters and runs all of the
// submodules, creating an updated thresholded image
void ColorTableCreator::updateThresholdedImage()
{
    topConverter.changeTable(colorTable.getTable());
    bottomConverter.changeTable(colorTable.getTable());
    // Run all of the modules that are kept in our subdiagram
    subdiagram.run();
    updateColorStats();
}

void ColorTableCreator::updateColorStats()
{
    int colorCount = colorTable.countColor(image::Color_bits[currentColor]);
    colorStats->setText("Color count: " + QVariant(colorCount).toString());
}

void ColorTableCreator::canvasClicked(int x, int y, int brushSize, bool leftClick)
{
    BrushStroke brushStroke(x, y, (image::Color::ColorID) currentColor, brushSize, leftClick);
    brushStrokes.push_back(brushStroke);
    paintStroke(brushStroke);
}

void ColorTableCreator::undo()
{
    if (brushStrokes.empty())
        return;

    BrushStroke reverseStroke = brushStrokes.back().invert();
    paintStroke(reverseStroke);
    brushStrokes.pop_back();
}

void ColorTableCreator::paintStroke(const BrushStroke& brushStroke)
{
    // Check the click was on the image
    for (int i = -brushStroke.brushSize/2; i <= brushStroke.brushSize/2; i++)
    {
        for (int j = -brushStroke.brushSize/2; j <= brushStroke.brushSize/2; j++)
        {
            int brush_x = i + brushStroke.x;
            int brush_y = j + brushStroke.y;

            messages::YUVImage image;

            if (currentCamera == Camera::TOP)
            {
                image = topImageIn.message();
            }
            else
            {
                image = bottomImageIn.message();
            }

            // Get the color from the image and emit it
            if(0 < brush_x && brush_x < image.width()/2 &&
               0 < brush_y && brush_y < image.height())
            {
                byte y = image.yImage().getPixel(brush_x, brush_y);
                byte u = image.uImage().getPixel(brush_x/2, brush_y);
                byte v = image.vImage().getPixel(brush_x/2, brush_y);

                //std::cout << (int) y << " " << (int) u << " " << (int) v
                //       << std::endl;

                if (brushStroke.define)
                {
                    colorTable.setColor(y, u, v,
                                        image::Color_bits[brushStroke.color]);
                }
                else
                {
                    colorTable.unSetColor(y, u, v,
                                          image::Color_bits[brushStroke.color]);
                }
            }
        }
    }
   updateThresholdedImage();
}

void ColorTableCreator::imageTabSwitched(int)
{
    // Rewire the thresholded display's inPortal to get the right thing
    if (imageTabs->currentWidget() == &topDisplay) {
        currentCamera = Camera::TOP;
        thrDisplay.imageIn.wireTo(&topConverter.thrImage);
    } else {
        currentCamera = Camera::BOTTOM;
        thrDisplay.imageIn.wireTo(&bottomConverter.thrImage);
    }

    loadLatestTable();
    updateThresholdedImage();
}

void ColorTableCreator::updateColorSelection(int color)
{
    currentColor = color;
    topDisplay.setBrushColor(QColor(image::Color_RGB[color]));
    bottomDisplay.setBrushColor(QColor(image::Color_RGB[color]));
    updateColorStats();
}

}
}
