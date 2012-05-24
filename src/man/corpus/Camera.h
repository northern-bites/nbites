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
        TOP = 0x01,
        BOTTOM = 0x02
    };

    static const int KEEP_DEFAULT = -1;

    struct Settings {

        int auto_gain;
        int gain;
        int auto_whitebalance;
        int blue_chroma;
        int red_chroma;
        int brightness;
        int contrast;
        int saturation;
        int hue;
        // Exposure length
        int auto_exposition;
        int exposure;
    };

    static const int DEFAULT_FRAMERATE = 30;

    static const int DEFAULT_AUTO_WHITEBALANCE = false; // AUTO WB OFF
    static const int DEFAULT_AUTO_EXPOSITION = false; // AUTO EXPOSURE OFF

#if ROBOT_TYPE == NAO_NEXTGEN
    static const int DEFAULT_AUTO_GAIN = KEEP_DEFAULT;
    static const int DEFAULT_GAIN = 40;
    static const int DEFAULT_BLUECHROMA = KEEP_DEFAULT;
    static const int DEFAULT_REDCHROMA = KEEP_DEFAULT;
    static const int DEFAULT_BRIGHTNESS = 80;
    static const int DEFAULT_CONTRAST = 60;
    static const int DEFAULT_SATURATION = 130;
    static const int DEFAULT_HUE = 0;
    static const int DEFAULT_EXPOSURE = 60;
#else
    static const int DEFAULT_AUTO_GAIN = false; // AUTO GAIN OFF
    static const int DEFAULT_GAIN = 27;
    static const int DEFAULT_BLUECHROMA = 120;
    static const int DEFAULT_REDCHROMA = 85;
    static const int DEFAULT_BRIGHTNESS = 150;
    static const int DEFAULT_CONTRAST = 88;
    static const int DEFAULT_SATURATION = 150;
    static const int DEFAULT_HUE = 0;
    static const int DEFAULT_EXPOSURE = 129;
#endif

    static const Settings& getDefaultSettings() {
        static const Settings defaultSettings = {
                DEFAULT_AUTO_GAIN,
                DEFAULT_GAIN,
                DEFAULT_AUTO_WHITEBALANCE,
                DEFAULT_BLUECHROMA,
                DEFAULT_REDCHROMA,
                DEFAULT_BRIGHTNESS,
                DEFAULT_CONTRAST,
                DEFAULT_SATURATION,
                DEFAULT_HUE,
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
