#pragma once

#include <Qt>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QtDebug>
#include <QMouseEvent>
#include "BMPImageViewer.h"

namespace qtool {
    namespace viewer {
        class BMPImageViewerListener: public BMPImageViewer {
             Q_OBJECT

             public:

             BMPImageViewerListener(image::BMPImage* image,
                                    QWidget *parent = NULL);
            void mouseReleaseEvent ( QMouseEvent *event);

            signals:
            void fetchColorToDefine(int x,int y);

        };
    }
}
