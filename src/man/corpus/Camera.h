/**
 * class that keeps camera information
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

namespace man {
namespace corpus {

class Camera {

public:
    enum Type {
        TOP = 0,
        BOTTOM = 1
    };

    struct Settings {
        Type type;

        int resolution;
        int framerate;
        int buffersize;

        bool auto_gain;
        int gain;
        bool auto_whitebalance;
        int blue_chroma;
        int red_chroma;
        int brightness;
        int contrast;
        int saturation;
        int hue;
        // Lens correction - unused as of now
        int lens_correction_x;
        int lens_correction_y;
        // Exposure length
        bool auto_exposition;
        int exposure;
    };


    // Color Settings
    // Gain: 26 / Exp: 83
    // Gain: 28 / Exp: 60
    // Gain: 35 / Exp: 40

    static const Type DEFAULT_TYPE = BOTTOM;

    static const int DEFAULT_RESOLUTION = 14;
    static const int DEFAULT_FRAMERATE = 30;
    static const int DEFAULT_BUFFERSIZE = 16;
    static const bool DEFAULT_AUTO_GAIN = false; // AUTO GAIN OFF
    static const int DEFAULT_GAIN = 30;
    static const bool DEFAULT_AUTO_WHITEBALANCE = false; // AUTO WB OFF
    static const int DEFAULT_BLUECHROMA = 113;
    static const int DEFAULT_REDCHROMA = 60;
    static const int DEFAULT_BRIGHTNESS = 154;
    static const int DEFAULT_CONTRAST = 85;
    static const int DEFAULT_SATURATION = 140;
    static const int DEFAULT_HUE = 0;
    static const int DEFAULT_LENSX = 0;
    static const int DEFAULT_LENSY = 0;
    static const bool DEFAULT_AUTO_EXPOSITION = false; // AUTO EXPOSURE OFF
    static const int DEFAULT_EXPOSURE = 129;


    static const Settings getDefaultSettings() {
        const Settings defaultSettings = {
                DEFAULT_TYPE,
                DEFAULT_RESOLUTION,
                DEFAULT_FRAMERATE,
                DEFAULT_BUFFERSIZE,
                DEFAULT_AUTO_GAIN,
                DEFAULT_GAIN,
                DEFAULT_AUTO_WHITEBALANCE,
                DEFAULT_BLUECHROMA,
                DEFAULT_REDCHROMA,
                DEFAULT_BRIGHTNESS,
                DEFAULT_CONTRAST,
                DEFAULT_SATURATION,
                DEFAULT_HUE,
                DEFAULT_LENSX,
                DEFAULT_LENSY,
                DEFAULT_AUTO_EXPOSITION,
                DEFAULT_EXPOSURE
        };
        return defaultSettings;
    }

    static const Type getOtherCameraType(Type type) {
        if (type == TOP) {
            return BOTTOM;
        } else {
            return TOP;
        }
    }
};

}
}
