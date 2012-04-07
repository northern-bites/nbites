#include "BMPImageViewerListener.h"


using namespace qtool::image;
using namespace man::memory;

namespace qtool {
    namespace viewer {

        BMPImageViewerListener::BMPImageViewerListener(image::BMPImage* image,
                                                       QWidget *parent)
            : BMPImageViewer(image,parent){
            setupUI();
            QWidget::setAttribute(Qt::WA_NoMousePropagation,true );
        }

        void BMPImageViewerListener::mouseReleaseEvent ( QMouseEvent * event )
        {
            if(event->button() == Qt::LeftButton)
            {
                qDebug() << QString::number(event->pos().x());
                qDebug() << QString::number(event->pos().y());

                emit fetchColorToDefine(event->pos().x(), event->pos().y());

            }
        }

    }
}
