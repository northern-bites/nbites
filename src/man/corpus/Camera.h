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
    static const int DEFAULT_BRIGHTNESS = KEEP_DEFAULT;
    static const int DEFAULT_CONTRAST = 60;
    static const int DEFAULT_SATURATION = 130;
    static const int DEFAULT_HUE = KEEP_DEFAULT;
    static const int DEFAULT_EXPOSURE = 60;
#else
    static const int DEFAULT_AUTO_GAIN = false; // AUTO GAIN OFF
    static const int DEFAULT_GAIN = 30;
    static const int DEFAULT_BLUECHROMA = 113;
    static const int DEFAULT_REDCHROMA = 60;
    static const int DEFAULT_BRIGHTNESS = 154;
    static const int DEFAULT_CONTRAST = 85;
    static const int DEFAULT_SATURATION = 140;
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
};

}
}
