
#include <QLabel>
#include <QLayout>
#include "ColorSpaceWidget.h"

namespace qtool {
namespace colorcreator {

ColorSpaceWidget::ColorSpaceWidget(ColorSpace* colorSpace, QWidget* parent)
    : QWidget(parent) {

    QVBoxLayout* layout = new QVBoxLayout;

    //add sliders and labels to the widget
    for (int i = 0; i < ColorSpace::NUM_CHANNELS; i++) {

        QHBoxLayout* inlineLayout = new QHBoxLayout;

        inlineLayout->addWidget(new QLabel(QString(fltChannel_names[i].c_str()), this));

        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, (int) FLOAT_SLIDER_SCALE);
        //this ensures it only sends a valueChanged signal once it gets released
        slider->setTracking(false);
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
        sliders.insert(slider, (ColorSpace::Channel) i);
        inlineLayout->addWidget(slider);

        layout->addLayout(inlineLayout);
    }

    this->setColorSpace(colorSpace);
	layout->setSpacing(1);
    this->setLayout(layout);
}

void ColorSpaceWidget::setColorSpace(ColorSpace* _colorSpace) {
    colorSpace = _colorSpace;
    //set slider values according to ColorSpace params
    QMapIterator<QSlider*, ColorSpace::Channel> it(sliders);
    while (it.hasNext()) {
        it.next();
        QSlider* slider = it.key();
        //block signals in order to feedback the value we set on the slider
        //back into the color (parameter) space
        slider->blockSignals(true);
        slider->setValue(colorSpace->getParameter(it.value())*FLOAT_SLIDER_SCALE);
        slider->blockSignals(false);
    }
}

void ColorSpaceWidget::sliderValueChanged(int value) {
    QSlider* sender = dynamic_cast<QSlider*>(QObject::sender());

    if (!sender)
        return;

    if (sliders.contains(sender)) {
        colorSpace->setParameter(sliders[sender], value / FLOAT_SLIDER_SCALE);
    }
}

}
}
