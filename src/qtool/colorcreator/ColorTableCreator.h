/**
 * @class ColorTableCreator
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QComboBox>

// colorcreator
#include "ColorTable.h"

#include "data/DataManager.h"
#include "image/BMPYUVImage.h"
#include "corpus/alconnect/ALConstants.h"
#include "corpus/ColorParams.h"
#include "corpus/ImageAcquisition.h"
//qtool
#include "viewer/BMPImageViewerListener.h"
#include "ColorEdit.h"
#include "ColorSpace.h"
#include "ColorSpaceWidget.h"
#include "ColorWheel.h"
#include "image/ThresholdedImage.h"

namespace qtool {
    namespace colorcreator {
        class ColorTableCreator : public QWidget
        {
            Q_OBJECT

        public:
            ColorTableCreator(qtool::data::DataManager::ptr dataManager,
                              QWidget *parent = 0);
            ~ColorTableCreator() {}

        public:
            static const image::ColorID STARTING_COLOR = image::Orange;

        protected slots:
            void loadColorTableBtnPushed();
            void saveColorTableBtnPushed();
            void updateThresholdedImage();
            void updateColorTable(byte y, byte u, byte v);
            void updateColorSelection(int color);

        private:
            data::DataManager::ptr dataManager;
            image::BMPYUVImage* image;
            image::ThresholdedImage* threshImage;
            viewer::BMPImageViewerListener* imageViewer;
            viewer::BMPImageViewer* thresholdedImageViewer;
            QPushButton saveColorTableBtn;
            ColorTable* colorTable;
            QComboBox colorSelect;
            int currentColor;
            ColorParams params;
            uint8_t* rawThresh;

            boost::shared_ptr<man::memory::proto::PImage> rawThreshImage;
            // Octavians idea for name


        };
    }
}
