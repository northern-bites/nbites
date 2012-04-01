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

// colorcreator
#include "ColorTable.h"

#include "data/DataManager.h"
#include "image/BMPYUVImage.h"

//qtool
#include "viewer/ChannelImageViewer.h"
#include "ColorEdit.h"
#include "ColorSpace.h"
#include "ColorSpaceWidget.h"
#include "ColorWheel.h"

namespace qtool {
    namespace colorcreator {
        class ColorTableCreator : public QWidget
        {
            Q_OBJECT

        public:
            ColorTableCreator(qtool::data::DataManager::ptr dataManager,
                              QWidget *parent = 0);
            ~ColorTableCreator() {}

        protected slots:
            void loadColorTableBtnPushed();
            void saveColorTableBtnPushed();

        private:
            data::DataManager::ptr dataManager;
            image::BMPYUVImage* currentImage;
            QPushButton nextImageBtn, previousImageBtn,
                saveColorTableBtn;

        };
    }
}
