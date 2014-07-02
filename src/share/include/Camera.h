/**
 * class that keeps camera information
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 * @author Lizzie Mamantov (2012)
 */

#pragma once

class Camera {

public:
    enum Type {
        TOP = 0x01,
        BOTTOM = 0x02
    };

    struct Settings {
        bool hflip;
        bool vflip;
        bool auto_exposure;
        int brightness;
        int contrast;
        int saturation;
        int hue;
        int sharpness;
        bool auto_whitebalance;
        int backlight_compensation;
        int exposure;
        int gain;
        int white_balance;
        bool fade_to_black;
    };

    static const Settings& getSettings(Type type)
        {
            if(type == TOP) return getTopSettings();
            else return getBottomSettings();
        }

    static const Settings& getTopSettings() {
        static const Settings topSettings = {
            TOP_H_FLIP,
            TOP_V_FLIP,
            TOP_AUTO_EXPOSURE,
            TOP_BRIGHTNESS,
            TOP_CONTRAST,
            TOP_SATURATION,
            TOP_HUE,
            TOP_SHARPNESS,
            TOP_AUTO_WHITEBALANCE,
            TOP_BACKLIGHT_COMPENSATION,
            TOP_EXPOSURE,
            TOP_GAIN,
            TOP_WHITE_BALANCE,
            TOP_FADE_TO_BLACK
        };
        return topSettings;
    }

    static const Settings& getBottomSettings() {
        static const Settings bottomSettings = {
            BOTTOM_H_FLIP,
            BOTTOM_V_FLIP,
            BOTTOM_AUTO_EXPOSURE,
            BOTTOM_BRIGHTNESS,
            BOTTOM_CONTRAST,
            BOTTOM_SATURATION,
            BOTTOM_HUE,
            BOTTOM_SHARPNESS,
            BOTTOM_AUTO_WHITEBALANCE,
            BOTTOM_BACKLIGHT_COMPENSATION,
            BOTTOM_EXPOSURE,
            BOTTOM_GAIN,
            BOTTOM_WHITE_BALANCE,
            BOTTOM_FADE_TO_BLACK
        };
        return bottomSettings;
    }
    /*
     * Camera Parameters Go Here
     */
    // FORT ANDROS CAMERA PARAMETERS
    // TOP SETTINGS
    static const bool TOP_H_FLIP = 1;
    static const bool TOP_V_FLIP = 1;
    static const bool TOP_AUTO_EXPOSURE = 0;
    static const int TOP_BRIGHTNESS = 200;
    static const int TOP_CONTRAST = 58;
    static const int TOP_SATURATION = 134;
    static const int TOP_HUE = 0;
    static const int TOP_SHARPNESS = 3;
    static const bool TOP_AUTO_WHITEBALANCE = 0;
    static const int TOP_BACKLIGHT_COMPENSATION = 0x00;
    static const int TOP_EXPOSURE = 50;
    static const int TOP_GAIN = 53;
    static const int TOP_WHITE_BALANCE = 3300;
    static const bool TOP_FADE_TO_BLACK = 0;

    // // BOTTOM SETTINGS
    static const bool BOTTOM_H_FLIP = 0;
    static const bool BOTTOM_V_FLIP = 0;
    static const bool BOTTOM_AUTO_EXPOSURE = 0;
    static const int BOTTOM_BRIGHTNESS = 200;
    static const int BOTTOM_CONTRAST = 56;
    static const int BOTTOM_SATURATION = 122;
    static const int BOTTOM_HUE = 0;
    static const int BOTTOM_SHARPNESS = 3;
    static const bool BOTTOM_AUTO_WHITEBALANCE = 0;
    static const int BOTTOM_BACKLIGHT_COMPENSATION = 0x00;
    static const int BOTTOM_EXPOSURE = 50;
    static const int BOTTOM_GAIN = 62;
    static const int BOTTOM_WHITE_BALANCE = 3200;
    static const bool BOTTOM_FADE_TO_BLACK = 0;

    // // WATSON 2014 CAMERA PARAMETERS
    // // TOP SETTINGS
    // static const bool TOP_H_FLIP = 1;
    // static const bool TOP_V_FLIP = 1;
    // static const bool TOP_AUTO_EXPOSURE = 0;
    // static const int TOP_BRIGHTNESS = 84;
    // static const int TOP_CONTRAST = 45;
    // static const int TOP_SATURATION = 120;
    // static const int TOP_HUE = 0;
    // static const int TOP_SHARPNESS = 3;
    // static const bool TOP_AUTO_WHITEBALANCE = 0;
    // static const int TOP_BACKLIGHT_COMPENSATION = 0x00;
    // static const int TOP_EXPOSURE = 43;
    // static const int TOP_GAIN = 62;
    // static const int TOP_WHITE_BALANCE = 5500;
    // static const bool TOP_FADE_TO_BLACK = 0;

    // // BOTTOM SETTINGS
    // static const bool BOTTOM_H_FLIP = 0;
    // static const bool BOTTOM_V_FLIP = 0;
    // static const bool BOTTOM_AUTO_EXPOSURE = 0;
    // static const int BOTTOM_BRIGHTNESS = 96;
    // static const int BOTTOM_CONTRAST = 45;
    // static const int BOTTOM_SATURATION = 124;
    // static const int BOTTOM_HUE = 0;
    // static const int BOTTOM_SHARPNESS = 3;
    // static const bool BOTTOM_AUTO_WHITEBALANCE = 0;
    // static const int BOTTOM_BACKLIGHT_COMPENSATION = 0x00;
    // static const int BOTTOM_EXPOSURE = 43;
    // static const int BOTTOM_GAIN = 65;
    // static const int BOTTOM_WHITE_BALANCE = 5500;
    // static const bool BOTTOM_FADE_TO_BLACK = 0;

    static const Type getOtherCameraType(Type type) {

        if (type == TOP) {
            return BOTTOM;
        } else {
            return TOP;
        }
    }
};
