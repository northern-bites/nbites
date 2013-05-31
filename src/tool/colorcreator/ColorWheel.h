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

namespace tool {
namespace colorcreator {
  using color::ColorSpace;

class ColorWheel : public QWidget {
    Q_OBJECT

	  //public:
    static const int DEFAULT_SIZE = 200;
    static const int DEFAULT_RADIUS = 100;
    static const float ZSLICE_RANGE = 256.0f;

public:
    ColorWheel(ColorSpace* _colorSpace, QWidget* parent = NULL);
    virtual ~ColorWheel() {}

    QSize minimumSizeHint() const { return sizeHint(); }
    QSize sizeHint() const {
        //hack to size this widget properly
        return QSize(DEFAULT_SIZE + zSlider.width() + 25, DEFAULT_SIZE);
    }

public slots:
    void updateWheel();
    void setColorSpace(ColorSpace* _colorSpace);

protected slots:
    void zSliceChanged(int value);

private:
    ColorSpace* colorSpace;
    QImage wheelImage;
    QLabel wheelImagePlaceholder;
    QSlider zSlider;
    float zSlice;

};

}
}
