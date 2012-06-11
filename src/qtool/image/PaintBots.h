/**
 * @class Viewer
 *
 * This class draws the locations of the robots on the field, using globally set field constants
 *
 * @author Brian Jacobel
 * @date April 2012
 *
 * Made it so it draws the location of the robots  to a bmp image; used as an overlay on top of PaintField
 */

#pragma once

#include <QtGui>
#include <vector>

#include "data/DataManager.h"
#include "image/BMPImage.h"
#include "man/include/FieldConstants.h"
#include "viewer/BotLocs.h"

namespace qtool {
namespace image {

class PaintBots : public image::BMPImage {
    Q_OBJECT

public:
    PaintBots(QObject *parent = 0, float scaleFactor = 1.0f);
    ~PaintBots(){}
    unsigned getWidth() const { return FIELD_WIDTH*scaleFactor; }
    unsigned getHeight() const { return FIELD_HEIGHT*scaleFactor; }

    //this should be moved somewhere else and not be public
    viewer::BotLocs* locs;

protected:
    virtual void buildBitmap();
    float scaleFactor;

};

}
}
