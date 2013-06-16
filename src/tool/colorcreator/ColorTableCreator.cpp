/**
 * @class ColorTableCreator
 */

#include "ColorTableCreator.h"
#include <QMouseEvent>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <QDebug>

namespace tool {
namespace color {

ColorTableCreator::ColorTableCreator(QWidget *parent) :
    QWidget(parent),
    currentCamera(Camera::TOP),
    topDisplay(this),
    bottomDisplay(this),
    thrDisplay(this),
    bottomImage(base()),
    topImage(base()),
    topThrImage(base()),
    botThrImage(base())
{
    // BACKEND
    // We need converter modules to threshold both the top and bottom images,
    // and ImageDisplayModule for each image, and a ThresholdedDisplayModule
    // for whichever image is currently being workied on
    subdiagram.addModule(topDisplay);
    subdiagram.addModule(bottomDisplay);
    subdiagram.addModule(thrDisplay);

    topDisplay.imageIn.wireTo(&topImage, true);
    bottomDisplay.imageIn.wireTo(&bottomImage, true);
    thrDisplay.imageIn.wireTo(&topThrImage, true);

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

    rightLayout->addWidget(&thrDisplay);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);
}

// This gets called every time the logs are advanced, ie every time the
// "forward" button is pressed in the main tool
void ColorTableCreator::run_()
{
    bottomImageIn.latch();
    topImageIn.latch();
    topThrIn.latch();
    botThrIn.latch();

    bottomImage.setMessage(portals::Message<messages::YUVImage>(
                               &bottomImageIn.message()));
    topImage.setMessage(portals::Message<messages::YUVImage>(
                            &topImageIn.message()));
    topThrImage.setMessage(portals::Message<messages::ThresholdImage>(
                               &topThrIn.message()));
    botThrImage.setMessage(portals::Message<messages::ThresholdImage>(
                               &botThrIn.message()));

    subdiagram.run();
}

// Updates the color tables for both image converters and runs all of the
// submodules, creating an updated thresholded image
void ColorTableCreator::updateThresholdedImage()
{
    // Run all of the modules that are kept in our subdiagram
    run_();
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

    std::vector<colorChanges> tableAdjustments;
    // Check the click was on the image
    for (int i = -brushStroke.brushSize; i <= 0; i++)
    {
        for (int j = 0; j <= brushStroke.brushSize; j++)
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
                // Change the radius' to determine how 'close' colors must be to get defined
                const int yRadius = 4, uRadius = 2, vRadius = 2;
                for (int dy = -yRadius; dy <= yRadius; ++dy) {
                    for (int du = -uRadius; du <= uRadius; ++du) {
                        for (int dv = -vRadius; dv <= vRadius; ++dv) {
                            colorChanges adjustment;
                            adjustment.y = y+dy;
                            adjustment.u = u+du;
                            adjustment.v = v+dv;
                            adjustment.color = image::Color_bits[brushStroke.color];
                            tableAdjustments.push_back(adjustment);
                        }
                    }
                }

            }
        }
    }
    if (brushStroke.define)
    {
        emit tableChanges(tableAdjustments);
    }
    else
    {
        emit tableUnChanges(tableAdjustments);
    }
    updateThresholdedImage();
}

void ColorTableCreator::imageTabSwitched(int index)
{
    // Check if there is a valid widget yet or not.
    // We get this signal when we close the tool.
    if (index == -1)
        return;

    // Rewire the thresholded display's inPortal to get the right thing
    if (imageTabs->currentWidget() == &topDisplay) {
        currentCamera = Camera::TOP;
        thrDisplay.imageIn.wireTo(&topThrImage, true);
    } else {
        currentCamera = Camera::BOTTOM;
        thrDisplay.imageIn.wireTo(&botThrImage, true);
    }

    updateThresholdedImage();
}

void ColorTableCreator::updateColorSelection(int color)
{
    currentColor = color;
    topDisplay.setBrushColor(QColor(image::Color_RGB[color]));
    bottomDisplay.setBrushColor(QColor(image::Color_RGB[color]));
}

}
}
