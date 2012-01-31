
#include "ColorWheel.h"

#include "NBMath.h"
#include "image/Color.h"
#include <cmath>

namespace qtool {
namespace colorcreator {

using namespace qtool::image;

ColorWheel::ColorWheel(ColorSpace* _colorSpace, QWidget* parent) :
        QWidget(parent), colorSpace(NULL) {

    QHBoxLayout* layout = new QHBoxLayout;

    wheelImage = new QImage(DEFAULT_SIZE, DEFAULT_SIZE, QImage::Format_RGB32);
    wheelImagePlaceholder = new QLabel;

    wheelImagePlaceholder->setPixmap(QPixmap::fromImage(*wheelImage));
    layout->addWidget(wheelImagePlaceholder);

    zSlider = new QSlider(this);
    zSlider->setRange(0, (int) ZSLICE_RANGE);
    connect(zSlider, SIGNAL(valueChanged(int)), this, SLOT(zSliceChanged(int)));
    layout->addWidget(zSlider);

    this->setLayout(layout);

    setColorSpace(_colorSpace);
}

void ColorWheel::setColorSpace(ColorSpace* _colorSpace) {

    if (colorSpace) {
        disconnect(colorSpace, 0, this, 0);
    }
    colorSpace = _colorSpace;
    connect(colorSpace, SIGNAL(parametersChanged()),
            this, SLOT(updateWheel()));

    zSlider->setValue((int) (colorSpace->getParameter(ColorSpace::hMin) +
                             colorSpace->getParameter(ColorSpace::hMax))/2);
    updateWheel();
}

void ColorWheel::zSliceChanged(int value) {
    zSlice = value / ZSLICE_RANGE;
    updateWheel();
}

void ColorWheel::updateWheel() {
    wheelImage->fill(Qt::white);
    for (int i = -DEFAULT_SIZE/2; i < DEFAULT_SIZE/2; i++)
    {
        for (int j = -DEFAULT_SIZE/2; j < DEFAULT_SIZE/2; j++)
        {
            float dist = sqrt(i * i + j * j);
            if (dist < 100.0)
            {
                float s = dist / (DEFAULT_SIZE/2.0f);
                float h = atan2(i, j) / (2 * PI);
                if (h < 0) {
                    h = 1.0f + h;
                }
                Color color;
                color.setHsz(h, s, zSlice);
                if (colorSpace->contains(color)) {
                    wheelImage->setPixel(i + DEFAULT_SIZE/2,
                                         j + DEFAULT_SIZE/2,
                                         color.getRGB());
                }
            }
        }
    }
    wheelImagePlaceholder->setPixmap(QPixmap::fromImage(*wheelImage));
}

}
}
