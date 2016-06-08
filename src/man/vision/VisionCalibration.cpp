#include "VisionCalibration.hpp"
#include "Log.hpp"

namespace man {
    namespace vision {

        const std::string nbites_dir() {
            const char * envVar = getenv("NBITES_DIR");
            if ( envVar ) {
                return std::string(envVar);
            } else {
                throw std::runtime_error("NBITES_DIR required for offline testing");
            }
        }

        const std::string colorParamsPath() {
#ifdef OFFLINE
            return nbites_dir() + nbl::SharedConstants::OFFLINE_COLOR_CALIBRATION_SUFFIX();
#else
            return nbl::SharedConstants::ONLINE_COLOR_CALIBRATION_PATH();
#endif
        }

        const std::string cameraOffsetsPath() {
#ifdef OFFLINE
            return nbites_dir() + nbl::SharedConstants::OFFLINE_CAMERA_OFFSET_SUFFIX();
#else
            return nbl::SharedConstants::ONLINE_COLOR_CALIBRATION_PATH();
#endif
        }

        
    }
}