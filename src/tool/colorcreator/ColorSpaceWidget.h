/**
 * @class ColorSpaceWidget
 *
 * Defines a GUI to control color space parameters
 *
 * @author Octavian Neamtu
 */
#pragma once

#include <QWidget>
#include <QMap>
#include <QSlider>
#include "image/Color.h"
#include "ColorSpace.h"

namespace tool {
namespace colorcreator {
  using color::ColorSpace;

class ColorSpaceWidget: public QWidget {

    Q_OBJECT;

public:
    static const float FLOAT_SLIDER_SCALE = 256.0f;

public:
    ColorSpaceWidget(ColorSpace* colorSpace, QWidget* parent = NULL);

    ~ColorSpaceWidget() {}

    void setColorSpace(ColorSpace* _colorSpace);

protected slots:
    void sliderValueChanged(int value);

private:
    QMap<QSlider*, ColorSpace::Channel> sliders;
    ColorSpace* colorSpace;
};

}
}
