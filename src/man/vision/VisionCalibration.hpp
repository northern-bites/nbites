#ifndef VISION_CALIBRATION_H
#define VISION_CALIBRATION_H

#include "Vision.h"
#include "Homography.h"
#include "FrontEnd.h"

#include "json.hpp"

namespace man {
    namespace vision {
        namespace calibration {

            const std::string cameraOffsetsPath();
            const std::string colorParamsPath();

            CalibrationParams * parseOffsetsFromJSON(json::Object& object);
            CalibrationParams * getSavedOffsets(const std::string& robotName, bool top,
                                                const std::string& string);
            CalibrationParams * getSavedOffsets(const std::string& robotName, bool top);

            json::Object serialize(CalibrationParams * params);
            //ColorParams are mangled by FrontEnd, so it is easier to keep around
            //the previously parsed json than actually recreate it.

            Colors * parseColorsFromJSON(json::Object& object);
            Colors * getSavedColors(bool top, const std::string& string, json::Object * used);
            Colors * getSavedColors(bool top);
        }
    }
}

#endif