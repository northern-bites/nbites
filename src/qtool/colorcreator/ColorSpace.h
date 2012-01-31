/**
 * @class ColorSpace
 *
 * Defines a chunk of the entire color space through some parameters
 * Useful to define a chunk of the color space defines one color
 * in thresholding
 *
 * @author Octavian Neamtu with code from Eric Chown and EJ Googins
 */
#pragma once

#include <string>
#include <QObject>
#include "image/Color.h"
#include "ClassHelper.h"

namespace qtool {
namespace colorcreator {

static const std::string fltChannel_names[] = {
        "hMin", "hMax",
        "sMin", "sMax",
        "zMin", "zMax"
};

static const std::string intChannel_names[] = {
        "yMin", "yMax",
        "vMin", "vMax"
};

class ColorSpace : public QObject {

    Q_OBJECT;
    ADD_NULL_INSTANCE(ColorSpace)

public:
    enum fltChannel {
        hMin, hMax,
        sMin, sMax,
        zMin, zMax,
        NUM_FLOAT_CHANNELS};

    enum intChannel {
        yMin, yMax,
        vMin, vMax,
        NUM_INT_CHANNELS};


public:

    ColorSpace() {
    }
    virtual ~ColorSpace() {}

    bool contains(image::Color color) const {

        // the circle can wrap around
        int h = color.getHb();
        int s = color.getSb();
        if (floatParams[hMin] > floatParams[hMax]) {
            if (floatParams[hMin] > h || floatParams[hMax] < h) {
                return false;
            }
        } else if (floatParams[hMin] > h && floatParams[hMax] < h) {
            return false;
        }
        if (s < floatParams[sMin] || s > floatParams[sMax]) {
            return false;
        }

        int y = color.getYb();
        int v = color.getVb();
        if (y < intParams[yMin] || y > intParams[yMax]) {
            return false;
        }
        if (v < intParams[vMin] || v > intParams[vMax]) {
            return false;
        }
        return true;
    }

    void setParameter(fltChannel channel, float value) {
        floatParams[channel] = value;
        emit parametersChanged();
    }
    void setParameter(intChannel channel, int value) {
        intParams[channel] = value;
        emit parametersChanged();
    }
    int getParameter(intChannel channel) {
        return intParams[channel];
    }
    float getParameter(fltChannel channel) {
        return floatParams[channel];
    }

signals:
    void parametersChanged();

private:
    float floatParams[NUM_FLOAT_CHANNELS];
    int   intParams[NUM_INT_CHANNELS];

};

}
}

