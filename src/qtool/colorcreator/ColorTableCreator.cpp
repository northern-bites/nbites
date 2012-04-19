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

        ColorTableCreator::ColorTableCreator(DataManager::ptr dataManager,
                                             QWidget *parent) :
            QWidget(parent), dataManager(dataManager),
            image(new BMPYUVImage(dataManager->getMemory()->getMImage(),
                                  BMPYUVImage::RGB, this)),
            // threshImage(new ThresholdedImage(dataManager->getMemory()
            //                                 ->getMImage(), this)),
            params(Y0, U0, V0, Y1, U1, V1, YLIMIT, ULIMIT, VLIMIT)
            // imageViewer(image, this)
        {
            rawThreshImage = shared_ptr<PImage>(new PImage);
            rawThreshImage->set_width(AVERAGED_IMAGE_WIDTH);
            rawThreshImage->set_height(AVERAGED_IMAGE_HEIGHT);

            // HACK HACK HACK
            rawThreshImage->mutable_image()->assign(COLOR_IMAGE_BYTE_SIZE,'0');
            rawThresh = (uint8_t*)(rawThreshImage->mutable_image()->data());

            threshImage = new ThresholdedImage(rawThreshImage, this);

            imageViewer = new viewer::BMPImageViewerListener(image, this);
            thresholdedImageViewer = new viewer::BMPImageViewer(threshImage, this);
            colorTable = new ColorTable();

            QHBoxLayout* mainLayout = new QHBoxLayout;
            QHBoxLayout* leftLayout = new QHBoxLayout;

            QHBoxLayout* rightLayout = new QHBoxLayout;

            dataManager->connectSlotToMObject(imageViewer,
                                              SLOT(updateView()),
                                              MIMAGE_ID);
            dataManager->connectSlotToMObject(thresholdedImageViewer,
                                              SLOT(updateView()),
                                              MIMAGE_ID);
            QObject::connect(imageViewer, SIGNAL(fetchColorToDefine(byte,byte,byte)),
                             this, SLOT(updateColorTable(byte,byte,byte)));

            //set up the color selection combo box
            for (int i = 0; i < image::NUM_COLORS; i++) {
                colorSelect.addItem(image::Color_label[i].c_str());
            }
            connect(&colorSelect, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(updateColorSelection(int)));
            colorSelect.setCurrentIndex(STARTING_COLOR);
            rightLayout->addWidget(&colorSelect);

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
            QString filename = QFileDialog::getOpenFileName
                                       (this,
                                        tr("Load Color Table from File"),
                                        base_directory,
                                        tr("Color Table files (*.tbl)"));
            // Use the filename
        }

        void ColorTableCreator::saveColorTableBtnPushed(){
            QString base_directory = QString(NBITES_DIR) + "/data/tables";
            QString filename = QFileDialog::getSaveFileName
                                       (this,
                                        tr("Save Color Table to File"),
                                        base_directory + "/new_sliders.tbl",
                                        tr("Color Table files (*.tbl)"));
            // Use the filename
        }

        void ColorTableCreator::updateThresholdedImage(){
            // current color table is 'colorTable'
            uint8_t* yuv = (uint8_t*) dataManager->getMemory()->getMImage()->get()->image().data();
            //qDebug() << "Start acquiring thresholded image";

//             ImageAcquisition::acquire_image_fast(colorTable->getLinearTable(),
//                                                   params,
//                                                   yuv,
//                                                   rawThresh);
//            byte** y_image = image->getYUVImage()->getYImage();
//            byte** u_image = image->getYUVImage()->getUImage();
//            byte** v_image = image->getYUVImage()->getVImage();
            for (int i = 0; i < rawThreshImage->height(); i++) {
                for (int j = 0; j < rawThreshImage->width(); j++) {
                    int thresh_index = rawThreshImage->width() * i + j;
                    rawThresh[thresh_index] = colorTable->getColor(
                            image->getYUVImage()->getY(2*i, 2*j),
                            image->getYUVImage()->getU(2*i, 2*j),
                            image->getYUVImage()->getV(2*i, 2*j));
                }
            }
            thresholdedImageViewer->updateView();
            //qDebug() << "No Segfault after the raw image is acquired";
        }

        void ColorTableCreator::updateColorTable(byte y, byte u, byte v)
        {
            colorTable->setColor(y,u,v,currentColor);
            updateThresholdedImage();
        }

        void ColorTableCreator::updateColorSelection(int color)
        {
            currentColor = color;
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
