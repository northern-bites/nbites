
#include <QLabel>
#include <QLayout>
#include "ColorSpaceWidget.h"

namespace qtool {
namespace colorcreator {

ColorSpaceWidget::ColorSpaceWidget(ColorSpace* colorSpace, QWidget* parent)
    : QWidget(parent) {

    QVBoxLayout* layout = new QVBoxLayout;

    //add sliders and labels to the widget
    for (int i = 0; i < ColorSpace::NUM_FLOAT_CHANNELS; i++) {

        QHBoxLayout* inlineLayout = new QHBoxLayout;

        inlineLayout->addWidget(new QLabel(QString(fltChannel_names[i].c_str()), this));

        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, (int) FLOAT_SLIDER_SCALE);
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
        floatSliders.insert(slider, (ColorSpace::fltChannel) i);
        inlineLayout->addWidget(slider);

        layout->addLayout(inlineLayout);
    }
    for (int i = 0; i < ColorSpace::NUM_INT_CHANNELS; i++) {

        QHBoxLayout* inlineLayout = new QHBoxLayout;

        inlineLayout->addWidget(new QLabel(QString(intChannel_names[i].c_str()), this));

        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, INT_SLIDER_MAX);
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
        intSliders.insert(slider, (ColorSpace::intChannel) i);
        inlineLayout->addWidget(slider);

        layout->addLayout(inlineLayout);
    }

    this->setColorSpace(colorSpace);
    this->setLayout(layout);
}

void ColorSpaceWidget::setColorSpace(ColorSpace* _colorSpace) {
    colorSpace = _colorSpace;
    //set slider values according to ColorSpace params
    QMapIterator<QSlider*, ColorSpace::fltChannel> fi(floatSliders);
    while (fi.hasNext()) {
        fi.next();
        fi.key()->setValue(colorSpace->getParameter(fi.value())*FLOAT_SLIDER_SCALE);
    }
    QMapIterator<QSlider*, ColorSpace::intChannel> ii(intSliders);
    while (ii.hasNext()) {
        ii.next();
        ii.key()->setValue(colorSpace->getParameter(ii.value()));
    }
}

void ColorSpaceWidget::sliderValueChanged(int value) {
    QSlider* sender = dynamic_cast<QSlider*>(QObject::sender());

    if (!sender)
        return;

    if (floatSliders.contains(sender)) {
        colorSpace->setParameter(floatSliders[sender], value / FLOAT_SLIDER_SCALE);
    }
    if (intSliders.contains(sender)) {
        colorSpace->setParameter(intSliders[sender], value);
    }
}

}
}
