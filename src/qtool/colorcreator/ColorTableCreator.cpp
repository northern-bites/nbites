/**
 * @class ColorTableCreator
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 */

#include "ColorTableCreator.h"


#include <QMouseEvent>

#include <QFileDialog>

namespace qtool {
    namespace colorcreator {
        using namespace qtool::data;
        using namespace qtool::image;

        ColorTableCreator::ColorTableCreator(DataManager::ptr dataManager,
                                             QWidget *parent) :
            QWidget(parent), dataManager(dataManager),
            image(new BMPYUVImage(dataManager->getMemory()->getMImage(),
                                  BMPYUVImage::RGB, this))
            // imageViewer(image, this)
        {
            imageViewer = new viewer::BMPImageViewerListener(image, this);
            rightImageViewer = new viewer::BMPImageViewer(image, this);
            QHBoxLayout* mainLayout = new QHBoxLayout;
            QHBoxLayout* leftLayout = new QHBoxLayout;

            QHBoxLayout* rightLayout = new QHBoxLayout;

            dataManager->connectSlotToMObject(imageViewer,
                                              SLOT(updateView()),
                                              MIMAGE_ID);
            dataManager->connectSlotToMObject(rightImageViewer,
                                              SLOT(updateView()),
                                              MIMAGE_ID);
            QObject::connect(imageViewer, SIGNAL(fetchColorToDefine(int,int)),
                             this, SLOT(updateColorTable(int,int)));

            leftLayout->addWidget(imageViewer);
            rightLayout->addWidget(rightImageViewer);
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
        }

        void ColorTableCreator::updateColorTable(int x, int y)
        {
            printf("X: %d, Y: %d\n",x,y);
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
