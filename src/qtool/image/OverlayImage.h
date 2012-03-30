/**
 * @class OverlayImage
 * 
 * Draws an overlay of the location of the ball and corners over the image in qtool.
 *
 */

#pragma once

#include "BMPImage.h"
#include <QImage>
#include "man/memory/MVision.h"

namespace qtool {
namespace image {


class OverlayImage : public BMPImage
{

 public:
    OverlayImage(man::memory::MVision::const_ptr visionData);
    virtual ~OverlayImage() {};

    void updateBitmap();
    void drawBall(const man::memory::proto::PVision::PVisualBall);
    void drawCorner(const man::memory::proto::PVision::PVisualCorner);
    void drawGoalPost(const man::memory::proto::PVision::PVisualFieldObject);
    void drawRedRobot(const man::memory::proto::PVision::PVisualRobot);
    void drawNavyRobot(const man::memory::proto::PVision::PVisualRobot);
    void drawLine(const man::memory::proto::PVision::PVisualLine);
    void drawCross(const man::memory::proto::PVision::PVisualCross);
    unsigned getWidth() {return 640;}
    unsigned getHeight() {return 480;}

 protected:
    man::memory::MVision::const_ptr visionData;
};

}
}
