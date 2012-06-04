/**
 * @class ColorTableCreator
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 */

#include "ColorTableCreator.h"
#include "corpus/alconnect/ALConstants.h"
#include <QMouseEvent>

#include <QFileDialog>

namespace qtool {
namespace colorcreator {
using namespace qtool::data;
using namespace qtool::image;
using namespace boost;
using namespace man::memory::proto;
using namespace man::corpus;

ColorTableCreator::ColorTableCreator(DataManager::ptr dataManager,
        QWidget *parent) :
        QWidget(parent), dataManager(dataManager),
        image(new BMPYUVImage(dataManager->getMemory()->getMImage(), BMPYUVImage::RGB, this)),
        sensors(new Sensors(shared_ptr<Speech>(new Speech()))),
        imageTranscribe(new OfflineImageTranscriber(sensors, dataManager->getMemory()->getMImage())),
        rawThresholdedImageData(new PImage())
{
    rawThresholdedImageData->set_width(AVERAGED_IMAGE_WIDTH);
    rawThresholdedImageData->set_height(AVERAGED_IMAGE_HEIGHT);

    threshImage = new ThresholdedImage(rawThresholdedImageData, this);

    imageViewer = new viewer::BMPImageViewerListener(image, this);
    thresholdedImageViewer = new viewer::BMPImageViewer(threshImage, this);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    QHBoxLayout* leftLayout = new QHBoxLayout;

    QVBoxLayout* rightLayout = new QVBoxLayout;

    dataManager->connectSlotToMObject(imageViewer,
            SLOT(updateView()),
            MIMAGE_ID);

    QObject::connect(imageViewer, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(canvassClicked(int, int, int, bool)));

    //set up the color selection combo box
    for (int i = 0; i < image::NUM_COLORS; i++) {
        colorSelect.addItem(image::Color_label[i].c_str());
    }
    connect(&colorSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateColorSelection(int)));
    colorSelect.setCurrentIndex(STARTING_COLOR);
    rightLayout->addWidget(&colorSelect);

    QPushButton* undoBtn = new QPushButton("Undo", this);
    connect(undoBtn, SIGNAL(clicked()), this, SLOT(undo()));
    rightLayout->addWidget(undoBtn);

    saveColorTableBtn.setText("Save Color Table");
    rightLayout->addWidget(&saveColorTableBtn);
    connect(&saveColorTableBtn, SIGNAL(clicked()),
            this, SLOT(saveColorTableBtnPushed()));

    leftLayout->addWidget(imageViewer);
    rightLayout->addWidget(thresholdedImageViewer);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);



    this->setLayout(mainLayout);
    // connect(image, SIGNAL(bitmapBuilt()),
            //         this, SLOT(updateThresholdedImage()));
    // leftLayout->addWidget(&imageViewer);

}

void ColorTableCreator::loadColorTableBtnPushed(){
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    // Use the filename
}

void ColorTableCreator::saveColorTableBtnPushed(){
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getSaveFileName(this,
                    tr("Save Color Table to File"),
                    base_directory + "/new_sliders.mtb",
                    tr("Color Table files (*.mtb)"));
}

void ColorTableCreator::updateThresholdedImage(){

    imageTranscribe->initTable(colorTable.getTable());
    imageTranscribe->acquireNewImage();
    //TODO: get better formatted image acquisition data
    rawThresholdedImageData->mutable_image()->assign(
            (const char*) sensors->getColorImage(),
            AVERAGED_IMAGE_SIZE);
    thresholdedImageViewer->updateView();
}


void ColorTableCreator::canvassClicked(int x, int y, int brushSize, bool leftClick) {

    BrushStroke brushStroke(x, y, (ColorID) currentColor, brushSize, leftClick);
    brushStrokes.push_back(brushStroke);
    this->paintMeLikeOneOfYourFrenchGirls(brushStroke);
}

void ColorTableCreator::undo() {

    if (brushStrokes.empty())
        return;

    BrushStroke reverseStroke = brushStrokes.back().invert();
    this->paintMeLikeOneOfYourFrenchGirls(reverseStroke);
    brushStrokes.pop_back();
}

void ColorTableCreator::paintMeLikeOneOfYourFrenchGirls(const BrushStroke& brushStroke) {

    // Check the click was on the image
    for (int i = -brushStroke.brushSize/2; i <= brushStroke.brushSize/2; i++) {
        for (int j = -brushStroke.brushSize/2; j <= brushStroke.brushSize/2; j++) {

            int brush_x = i + brushStroke.x;
            int brush_y = j + brushStroke.y;

            // Get the color from the image and emit it
            if(0 < brush_x && brush_x < image->getWidth() &&  0 < brush_y && brush_y < image->getHeight()) {

                byte y = image->getYUVImage()->getY(brush_x, brush_y);
                byte u = image->getYUVImage()->getU(brush_x, brush_y);
                byte v = image->getYUVImage()->getV(brush_x, brush_y);

                if (brushStroke.define) {
                    colorTable.setColor(y, u, v, image::Color_bits[brushStroke.color]);
                } else {
                    colorTable.unSetColor(y, u, v, image::Color_bits[brushStroke.color]);
                }
            }
        }
    }

    updateThresholdedImage();
}

void ColorTableCreator::updateColorSelection(int color)
{
    currentColor = color;
    imageViewer->setBrushColor(QColor(image::Color_RGB[color]));
}
}
}

// void ColorTableCreator::saveColorTableBtnPushed() {
//     QString base_directory = QString(NBITES_DIR) + "/data/tables";
//     QString filename =
//             QFileDialog::getSaveFileName(this,
//                     tr("Save Color Table to File"),
//                     base_directory + "/new_table.mtb",
//                     tr("Color Tables(*.mtb)"));
//     ColorTableCreator::writeFromSliders(filename, colorSpace);
// }
