#include "BMPImageViewerListener.h"


using namespace qtool::image;
using namespace man::memory;

namespace qtool {
    namespace viewer {

        BMPImageViewerListener::BMPImageViewerListener(image::BMPYUVImage* image,
                                                       QWidget *parent)
            : BMPImageViewer(image,parent){
            setupUI();
            QWidget::setAttribute(Qt::WA_NoMousePropagation,true );
            givenImage = image;
        }

        void BMPImageViewerListener::mouseReleaseEvent ( QMouseEvent * event )
        {
            qDebug() << "emit a mouse event";


            if(event->button() == Qt::LeftButton)
            {
                // Check the click was on the image
                if((event->pos().x() < image->getWidth()) && event->pos().y() < image->getHeight())
                {
                    // Get the color from the image and emit it
                    byte y = givenImage->getYUVImage()->getY(event->pos().x(),
                                                        event->pos().y());
                    byte u = givenImage->getYUVImage()->getU(event->pos().x(),
                                                        event->pos().y());
                    byte v = givenImage->getYUVImage()->getV(event->pos().x(),
                                                        event->pos().y());

                    emit fetchColorToDefine(y,u,v);
                }

            }
        }

    }
}
