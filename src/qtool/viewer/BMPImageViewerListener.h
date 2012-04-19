#pragma once

#include <Qt>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QtDebug>
#include <QMouseEvent>
#include "BMPImageViewer.h"
#include "../image/BMPYUVImage.h"

namespace qtool {
    namespace viewer {
        class BMPImageViewerListener: public BMPImageViewer {
             Q_OBJECT

             public:

             BMPImageViewerListener(image::BMPYUVImage* image,
                                    QWidget *parent = NULL);
            void mouseReleaseEvent ( QMouseEvent *event);

            signals:
            void fetchColorToDefine(byte y, byte u, byte v);

            private:
            unsigned width;
            unsigned height;
            image::BMPYUVImage* givenImage;

        };
    }
}
