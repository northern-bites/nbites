#pragma once

#include "RoboGrams.h"
#include "Images.h"
#include "Color.h"
#include <QLabel>
#include <QImage>

namespace tool {
namespace image {

enum ChannelType {
    RGB,
    Y,
    U,
    V,
    Red,
    Green,
    Blue,
    Hue,
    Saturation,
    Value,
    NUM_CHANNELS
};

static const std::string ChannelType_label[] = {
    "RGB",
    "Y",
    "U",
    "V",
    "Red",
    "Green",
    "Blue",
    "Hue",
    "Saturation",
    "Value"
};

class ImageDisplayModule : public QLabel, public portals::Module
{
    Q_OBJECT;

public:
    ImageDisplayModule(QWidget* parent = 0);

    portals::InPortal<messages::YUVImage> imageIn;

    ChannelType getChannelType() { return channel; }
    std::string getChannelLabel() { return ChannelType_label[channel]; }
    int width() { return imageIn.message().width(); }
    int height() { return imageIn.message().height(); }

protected:
    virtual void run_();

    QImage makeImageOfChannel(ChannelType channel);

    ChannelType channel;
};

}
}
