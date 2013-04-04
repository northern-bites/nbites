#pragma once

#include "RoboGrams.h"
#include "Images.h"
#include <QLabel>

namespace tool {
namespace image {

typedef messages::PackedImage<unsigned int> RGBImage;

class ImageDisplayQModule : public QLabel, public portals::Module
{
    Q_OBJECT;

public:
    ImageDisplayQModule(QWidget* parent = 0);

    portals::InPortal<RGBImage> imageIn;

protected:
    virtual void run_();
};

}
}
