/**
 * @class VisualInfoImage
 * 
 * Draws an overlay of the location of the ball and corners over the image in qtool.
 *
 */

#pragma once

#include "BMPImage.h"
#include <QImage>
#include "man/memory/MVision.h"
#include "man/corpus/Camera.h"

namespace qtool {
namespace image {


class VisualInfoImage : public BMPImage
{

 public:
  VisualInfoImage(man::memory::MVision::const_ptr visionData, man::corpus::Camera::Type camera);
    virtual ~VisualInfoImage() {};

    void buildBitmap();
    void drawBall(const man::memory::proto::PVision::PVisualBall);
    void drawCorner(const man::memory::proto::PVision::PVisualCorner);
    void drawGoalPost(const man::memory::proto::PVision::PVisualFieldObject);
    void drawRedRobot(const man::memory::proto::PVision::PVisualRobot);
    void drawNavyRobot(const man::memory::proto::PVision::PVisualRobot);
    void drawLine(const man::memory::proto::PVision::PVisualLine);
    void drawCross(const man::memory::proto::PVision::PVisualCross);
    unsigned getWidth() const {return 640;}
    unsigned getHeight() const {return 480;}
    
 protected:
    man::memory::MVision::const_ptr visionData;
    man::corpus::Camera::Type camera;
};

}
}
