
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef Sensors_H
#define Sensors_H
#include <string>
#include <vector>
#include <list>
#include <pthread.h>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

#include "SensorDef.h"
#include "SensorConfigs.h"
#include "VisionDef.h"
#include "Provider.h"
#include "Speech.h"
#include "BulkMonitor.h"
#include "include/synchro/mutex.h"
#include "Kinematics.h"
#include "memory/RoboImage.h"

enum SupportFoot {
    LEFT_SUPPORT = 0,
    RIGHT_SUPPORT
};

class Sensors;

enum SensorsEvent {
    NEW_MOTION_SENSORS = 1,
    NEW_VISION_SENSORS,
    NEW_IMAGE
};

struct FSR {
    FSR()
        : frontLeft(0), frontRight(0), rearLeft(0), rearRight(0) { }

    FSR(const float fl, const float fr,
        const float rl, const float rr)
        : frontLeft(fl), frontRight(fr), rearLeft(rl), rearRight(rr) { }

    float frontLeft;
    float frontRight;
    float rearLeft;
    float rearRight;
};

struct FootBumper {
    FootBumper() : left(0.0f), right(0.0f) {}
    // Since the foot bumpers only have pressed/unpressed states, but are stored
    // as floats with values 0.0f and 1.0f, we just test whether the value is
    // bigger than a half or not and assign a boolean based on that.
    FootBumper(const float _left, const float _right) {
        left = _left > 0.5f;
        right = _right > 0.5f;
    }

    bool left;
    bool right;
};

struct Inertial {
    Inertial()
        : accX(0), accY(0), accZ(0),
          gyrX(0), gyrY(0), angleX(0), angleY(0) { }

    Inertial(const float _accX, const float _accY, const float _accZ,
             const float _gyrX, const float _gyrY,
             const float _angleX, const float _angleY)
        : accX(_accX), accY(_accY), accZ(_accZ),
          gyrX(_gyrX), gyrY(_gyrY), angleX(_angleX), angleY(_angleY) { }

    float accX;
    float accY;
    float accZ;
    float gyrX;
    float gyrY;
    float angleX;
    float angleY;
};


class Sensors : public Provider<SensorsEvent>{
    //friend class Man;
public:
    Sensors(boost::shared_ptr<Speech> s);
    virtual ~Sensors();

    // Locking data retrieval methods
    //   Each of these methods first locks the associated mutex, copies the
    //   requested values, then unlocks the mutex before returning
    const std::vector<float> getBodyAngles() const;
    const std::vector<float> getBodyAngles_degs() const;
    const std::vector<float> getVisionBodyAngles() const;
    float getVisionAngle(Kinematics::JointName joint) const;
    const std::vector<float> getMotionBodyAngles() const;
    const std::vector<float> getMotionBodyAngles_degs() const;
    const std::vector<float> getBodyTemperatures() const;
    float getBodyAngle(Kinematics::JointName joint) const;
    const std::vector<float> getBodyAngleErrors() const ;
    float getBodyAngleError(Kinematics::JointName joint) const;
    const FSR getLeftFootFSR() const;
    const FSR getRightFootFSR() const;
    const FootBumper getLeftFootBumper() const;
    const FootBumper getRightFootBumper() const;
    const Inertial getInertial() const;
    const Inertial getInertial_degs() const;
    const Inertial getUnfilteredInertial() const;
    const float getUltraSoundLeft() const;
    const float getUltraSoundRight() const;
    const float getUltraSoundLeft_cm() const;
    const float getUltraSoundRight_cm() const;
    const SupportFoot getSupportFoot() const;
    const float getChestButton() const;
    const float getBatteryCharge() const;
    const float getBatteryCurrent() const;

    // Locking data storage methods
    //   Each of these methods first locks the associated mutex, stores
    //   the specified values, then unlocks the mutex before returning
    void setBodyAngles(float* jointTPointers[]);
    void setVisionBodyAngles(const std::vector<float>& v);
    void setMotionBodyAngles(const std::vector<float>& v);
    void setBodyAngleErrors(const std::vector<float>& v);
    void setBodyTemperatures(float* jointTPointers[]);
    void setLeftFootFSR(const float frontLeft, const float frontRight,
                        const float rearLeft, const float rearRight);
    void setRightFootFSR(const float frontLeft, const float frontRight,
                         const float rearLeft, const float rearRight);
    void setFSR(const FSR &leftFootFSR, const FSR &rightFootFSR);
    void setLeftFootBumper(const float left, const float right);
    void setLeftFootBumper(const FootBumper& bumper);
    void setRightFootBumper(const float left, const float right);
    void setRightFootBumper(const FootBumper& bumper);
    void setInertial(const float accX, const float accY, const float accZ,
                     const float gyrX, const float gyrY,
                     const float angleX, const float angleY);
    void setInertial(const Inertial &inertial);
    void setUnfilteredInertial(const float accX, const float accY,
                               const float accZ,
                               const float gyrX, const float gyrY,
                               const float angleX, const float angleY);
    void setUnfilteredInertial(const Inertial &inertial);
    void setUltraSound(const float l_dist, const float r_dist);
    void setSupportFoot(const SupportFoot _supportFoot);

    void setMotionSensors(const FSR &_leftFoot, const FSR &_rightFoot,
                          const float chestButton,
                          const Inertial &_inertial,
                          const Inertial &_unfiltered_inertial);

    void setVisionSensors(const FootBumper &_leftBumper,
                          const FootBumper &_rightBumper,
                          const float ultraSoundLeft,
                          const float ultraSoundRight,
                          const float batteryCharge,
                          const float batteryCurrent);

    // special methods
    //   the image retrieval and locking methods are a little different, as we
    //   don't copy the raw image data.  If locking is needed for some period
    //   of time while processing, the lockImage() method is called, followed
    //   by getImage() and MUST be followed finally by relaseImage().  The
    //   getImage() method will always retrieve the latest image pointer, but
    //   it is only guaranteed to be valid and unmodified between calls to
    //   lockImage() and releaseImage().
    //   Also of import is that the locking is only valid if the source of the
    //   image pointer decides to abide by it.  setImage() does not not lock on
    //   its own, and there is no way, even with locking, to guarantee that the
    //   underlying data at the image pointer location is not modified while
    //   the image is locked in Sensors.
    const uint8_t* getNaoImage() const;
    uint8_t* getWriteableNaoImage();
    const uint16_t* getYImage() const;
    const uint16_t* getImage() const;
    const uint16_t* getUVImage() const;
    const uint8_t* getColorImage() const;
    void setNaoImagePointer(char* img);
    void notifyNewNaoImage();
    const man::memory::RoboImage* getRoboImage() const;
    void setImage(const uint16_t* img);
    void lockImage() const;
    void releaseImage() const;

    // The following method will internally save a snapshot of the current body
    // angles. This way we can save joints that are synchronized to the most
    // current image. At the same time, the bodyAngles vector will still have the
    // most recent angles if some other module needs them.
    void updateVisionAngles();

    // Save a vision frame with associated sensor data
    void saveFrame();
    void loadFrame(std::string path);
    void resetSaveFrame();
    void startSavingFrames();
    void stopSavingFrames();
    bool isSavingFrames() const;

    // writes data collected the variance monitor to ~/naoqi/log/
    void writeVarianceData();
    // checks whether the sensors we're monitoring are "healthy" or not
    float percentBrokenFSR();
    float percentBrokenMotionSensors();
    bool angleXYBroken();
    float percentBrokenSonar();

private:
    void add_to_module();

    // put the sensor data values into the variance tracker, at the correct hz
    void updateMotionDataVariance();
    void updateVisionDataVariance();

    // Pointer to speech, for Sensor warnings
    boost::shared_ptr<Speech> speech;

    // Locking mutexes
    mutex angles_mutex;
    mutex vision_angles_mutex;
    mutex motion_angles_mutex;
    mutex errors_mutex;
    mutex temperatures_mutex;
    mutex fsr_mutex;
    mutex button_mutex;
    mutex bumper_mutex;
    mutex inertial_mutex;
    mutex unfiltered_inertial_mutex;
    mutex ultra_sound_mutex;
    mutex support_foot_mutex;
    mutex battery_mutex;
    mutex image_mutex;
    mutex variance_mutex;
    multi_mutex vision_sensors_mutex;
    multi_mutex motion_sensors_mutex;

    // Joint angles and sensors
    // Make the following distinction: bodyAngles is a vector of the most current
    // angles. visionBodyAngles is a snapshot of what the most current angles
    // were when the last vision frame started.
    std::vector<float> bodyAngles;
    std::vector<float> visionBodyAngles;
    std::vector<float> motionBodyAngles;
    std::vector<float> bodyAnglesError;
    std::vector<float> bodyTemperatures;

    // FSR sensors
    FSR leftFootFSR;
    FSR rightFootFSR;
    // Feet bumper sensors
    FootBumper leftFootBumper;
    FootBumper rightFootBumper;
    // Inertial sensors
    Inertial inertial;
    // Sonar sensors
    float ultraSoundDistanceLeft;
    float ultraSoundDistanceRight;

    const uint16_t *yImage, *uvImage;
    const uint8_t *colorImage;
    uint8_t *naoImage;
    man::memory::RoboImage roboImage;

    // Pose needs to know which foot is on the ground during a vision frame
    // If both are on the ground (DOUBLE_SUPPORT_MODE/not walking), we assume
    // left foot is on the ground.
    SupportFoot supportFoot;

    /**
     * Stuff below is not logged to vision frames or sent over the network to
     * TOOL.
     */

    // Sensor variance/health monitor
    BulkMonitor varianceMonitor, fsrMonitor;

    Inertial unfilteredInertial;
    //ChestButton
    float chestButton;
    //Battery
    float batteryCharge;
    float batteryCurrent;

    static int saved_frames;
    std::string FRM_FOLDER;
    bool saving_frames_on;
};


#endif /* Sensors_H */
