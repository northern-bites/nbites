/**
 * @class ColorWheel
 *
 * A square widget with an embedded Hsv colorwheel
 *
 * @author Octavian Neamtu
 * @author Eric Chown
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QLayout>
#include "ColorSpace.h"

namespace qtool {
namespace colorcreator {

class ColorWheel : public QWidget {
    Q_OBJECT

public:
    static const int DEFAULT_SIZE = 200;
    static const float ZSLICE_RANGE = 256.0f;

public:
    ColorWheel(ColorSpace* _colorSpace = ColorSpace::NullInstance(),
               QWidget *parent = NULL);
    virtual ~ColorWheel() {}

    QSize minimumSizeHint() const { return sizeHint(); }
    QSize sizeHint() const { return QSize(DEFAULT_SIZE, DEFAULT_SIZE); }

protected slots:
    void updateWheel();
    void zSliceChanged(int value);
    void setColorSpace(ColorSpace* _colorSpace);

private:
    ColorSpace* colorSpace;
    QImage* wheelImage;
    QLabel* wheelImagePlaceholder;
    QSlider* zSlider;
    float zSlice;

};

}
}
