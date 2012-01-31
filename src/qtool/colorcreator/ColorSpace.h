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
    enum Channel {
        hMin, hMax,
        sMin, sMax,
        zMin, zMax,
        yMin, yMax,
        vMin, vMax,
        NUM_CHANNELS };


public:

    ColorSpace() {
    }
    virtual ~ColorSpace() {}

    bool contains(image::Color color) const {

        // the circle can wrap around
        int h = color.getHb();
        int s = color.getSb();
        if (params[hMin] > params[hMax]) {
            if (params[hMin] > h || params[hMax] < h) {
                return false;
            }
        } else if (params[hMin] > h && params[hMax] < h) {
            return false;
        }
        if (s < params[sMin] || s > params[sMax]) {
            return false;
        }

        int y = color.getYb();
        int v = color.getVb();
        if (y < params[yMin] || y > params[yMax]) {
            return false;
        }
        if (v < params[vMin] || v > params[vMax]) {
            return false;
        }
        return true;
    }

    void setParameter(Channel channel, float value) {
        params[channel] = value;
        emit parametersChanged();
    }

    float getParameter(Channel channel) {
        return params[channel];
    }

signals:
    void parametersChanged();

private:
    float params[NUM_CHANNELS];

};

}
}

