#include "BMPImageViewerListener.h"


using namespace qtool::image;
using namespace man::memory;

namespace qtool {
namespace viewer {

BMPImageViewerListener::BMPImageViewerListener(image::BMPYUVImage* image,
        QWidget *parent)
    : BMPImageViewer(image,parent), brushSize(DEFAULT_BRUSH_SIZE) {

    QWidget::setAttribute(Qt::WA_NoMousePropagation, true );
}

void BMPImageViewerListener::mouseReleaseEvent ( QMouseEvent * event )
{
    bool left;
    if(event->button() == Qt::LeftButton) {
        left = true;
    } else {
        left = false;
    }

    int mouseX = event->x();
    int mouseY = event->y();

    emit mouseClicked(mouseX, mouseY, brushSize, left);
}

void BMPImageViewerListener::wheelEvent(QWheelEvent* event) {
    if (event->delta() > 0) {
        brushSize++;
    } else {
        brushSize--;
    }

    if (brushSize == 0) {
        brushSize = 1;
    }

    updateBrushCursor();
}

void BMPImageViewerListener::updateBrushCursor() {
    QPixmap cursor(brushSize, brushSize);

    cursor.fill(brushColor);
    this->setCursor(QCursor(cursor, brushSize, 0)); // not exactly sure why this works
}

}
}
