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
        "zMin", "zMax",
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
        float h = color.getH();
        float s = color.getS();
        float z = color.getZ();

		// we sometimes get bad values of Z
		if (z > 1.0f) {
			z = 1.0f;
		}
        if (params[hMin] > params[hMax]) {
            if (params[hMax] < h && h < params[hMin]) {
                return false;
            }
        } else if (params[hMax] < h || h < params[hMin]) {
            return false;
        }
        if (s < params[sMin] || s > params[sMax]) {
            return false;
        }
        if (z < params[zMin] || z > params[zMax]) {
            return false;
        }

        float y = color.getY();
        // add 0.5f to normalize to a 0 to 1 range
        float v = color.getV() + 0.5f;
        if (y < params[yMin] || y >= params[yMax]) {
            return false;
        }
        if (v < params[vMin] || v >= params[vMax]) {
            return false;
        }
        return true;
    }

    void setParameters(float* values) {
        setParametersSilently(values);
        emit parametersChanged();
    }

    void setParametersSilently(float* values) {
        memcpy(params, values, sizeof(float) * NUM_CHANNELS);
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

