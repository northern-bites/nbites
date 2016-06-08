#ifndef VISION_CALIBRATION_H
#define VISION_CALIBRATION_H

#include "Vision.h"
#include "Homography.h"
#include "FrontEnd.h"

#include "json.hpp"

namespace man {
    namespace vision {
//        loadColorParams(Colors)

        const std::string colorParamsPath();
        const std::string cameraOffsetsPath();

        Colors * parseColorsFromJSON(json::Object& object);
        Colors * getSavedColors(const std::string& robotName);

        CalibrationParams * parseOffsetsFromJSON(json::Object& object);
        CalibrationParams * getSavedOffsets(const std::string& robotName);
    }
}

#endif