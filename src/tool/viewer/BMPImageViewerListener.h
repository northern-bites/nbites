#pragma once

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QtDebug>
#include <QMouseEvent>
#include "BMPImageViewer.h"
#include "image/BMPYUVImage.h"

namespace qtool {
namespace viewer {

class BMPImageViewerListener: public BMPImageViewer {
    Q_OBJECT

    static const int DEFAULT_BRUSH_SIZE = 10;

public:

    BMPImageViewerListener(image::BMPImage* image,
            QWidget *parent = NULL);
    void mouseReleaseEvent ( QMouseEvent *event);
    // Scroll up or down to increase or decrease brush size
    void wheelEvent(QWheelEvent* event);

    void setBrushColor(QColor _brushColor) { brushColor = _brushColor; updateBrushCursor();}
    int getBrushSize() { return brushSize; }

signals:
    void mouseClicked(int x, int y, int brushSize, bool leftClick);

private:
    void updateBrushCursor();

private:
    QColor brushColor;
    int brushSize;

};
}
}
