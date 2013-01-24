#pragma once

#include <QObject>
#include "RoboGrams.h"
#include "World.h"
#include "Image.h"

namespace tool{
namespace visionsim{

class VisionSimModule :  public QObject,
                         public portals::Module
{
    Q_OBJECT;

public:
    VisionSimModule();
    virtual ~VisionSimModule() {}

public slots:
    void updateRobot(float x, float y, float h);
    void updateHead(float yaw, float pitch);
    void updateBall(float x, float y);

protected:
    virtual void run_();

    World world;
    Image topImage;
    Image bottomImage;
};

}
}
