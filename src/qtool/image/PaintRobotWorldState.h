/**
 * @class PaintVisionField
 *
 * This class draws vision objects on the field as the robot perceives them
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QtGui>
#include <vector>

#include "data/DataManager.h"
#include "image/PaintFieldObjects.h"
#include "man/include/FieldConstants.h"

#include "data/Typedefs.h"

namespace qtool {
namespace image {

class PaintRobotWorldState : public PaintFieldOverlay {

    Q_OBJECT

public:
    PaintRobotWorldState(memory::MVision::const_ptr ,
                         memory::MLocalization::const_ptr, QObject *parent = 0);
    ~PaintRobotWorldState(){}

protected:
    void draw(QPainter& painter,
              const proto::PVision::PVisualDetection& visualDetection,
              QColor color);
    virtual void buildBitmap();

protected:
    memory::MLocalization::const_ptr mLoc;
    memory::MVision::const_ptr mVision;

};

}
}
