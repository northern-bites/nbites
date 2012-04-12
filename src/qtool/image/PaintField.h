/**
 * @class Viewer
 *
 * This class draws the field so that we can display the
 * movement of robots and the ball on the field.  The 
 * painting uses all the constants from FieldConstants
 * so that if the field changes dimensions it should
 * still work.
 *
 * @author Dani McAvoy
 * @date October 2011
 *
 * @modified Octavian Neamtu
 * @date February 2012
 *
 * Made it so it draws the field to a bmp image; anything else
 * should go into an overlay on top of this
 */

#pragma once

#include <QtGui>
#include <vector>

#include "image/BMPImage.h"
#include "man/include/FieldConstants.h"

namespace qtool {
namespace image {

class PaintField : public image::BMPImage {

    Q_OBJECT

public:
    PaintField(QObject *parent = 0);

    unsigned getWidth() const { return FIELD_WIDTH; }
    unsigned getHeight() const { return FIELD_HEIGHT; }

protected:
    virtual void buildBitmap();

};

}
}
