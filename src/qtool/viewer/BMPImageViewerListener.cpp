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
        int mouse_x = event->pos().x();
        int mouse_y = event->pos().y();
        // Check the click was on the image
        for (int i = -10; i <= 10; i++) {
            for (int j = -10; j <= 10; j++) {

                int x = i + mouse_x;
                int y = j + mouse_y;

                // Get the color from the image and emit it
                if(0 < x && x < image->getWidth() &&  0 < y && y < image->getHeight()) {

                    byte y = givenImage->getYUVImage()->getY(x, y);
                    byte u = givenImage->getYUVImage()->getU(x, y);
                    byte v = givenImage->getYUVImage()->getV(x, y);
                    emit fetchColorToDefine(y,u,v);
                }
            }
        }
    }
}
}
}
