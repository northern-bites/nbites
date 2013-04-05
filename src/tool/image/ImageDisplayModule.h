#pragma once

#include "RoboGrams.h"
#include "Images.h"
#include <QLabel>

namespace tool {
namespace image {

class ImageDisplayModule : public QLabel, public portals::Module
{
    Q_OBJECT;

public:
    ImageDisplayModule(QWidget* parent = 0);

    portals::InPortal<messages::YUVImage> imageIn;

protected:
    virtual void run_();
};

}
}
