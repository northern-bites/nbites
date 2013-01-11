
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

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <sys/stat.h>

using namespace std;

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include <boost/lambda/lambda.hpp>
using namespace boost::lambda;

#include "Sensors.h"

#include "corpusconfig.h"
#include "NBMath.h"
#include "Kinematics.h"
using namespace Kinematics;
using namespace man::corpus;
using namespace man::memory;

// static base image array, so we don't crash on image access if the setImage()
// method is never called
static uint16_t global_image[NAO_IMAGE_BYTE_SIZE];

//
// C++ Sensors class methods
//
Sensors::Sensors(boost::shared_ptr<Speech> s,
                 MVisionSensors::ptr mVisionSensors,
                 MMotionSensors::ptr mMotionSensors) :
        speech(s),
        angles_mutex("angles"),
        vision_angles_mutex("vision_angles"),
        motion_angles_mutex("motion_angles"),
        errors_mutex("errors"),
        temperatures_mutex("temperatures"),
        fsr_mutex("fsr"),
        button_mutex("button"),
        bumper_mutex("bumper"),
        inertial_mutex("inertial"),
        unfiltered_inertial_mutex("unfiltered_inertial"),
        ultra_sound_mutex("ultra_sound"),
        support_foot_mutex("support_foot"),
        battery_mutex("battery"),
        image_mutex("image"),
        variance_mutex("variance"),
        bodyAngles(Kinematics::NUM_JOINTS, 0.0f),
        visionBodyAngles(Kinematics::NUM_JOINTS, 0.0f),
        motionBodyAngles(Kinematics::NUM_JOINTS, 0.0f),
        bodyAnglesError(Kinematics::NUM_JOINTS, 0.0f),
        bodyTemperatures(Kinematics::NUM_JOINTS, 0.0f),
        yBottomImage(&global_image[0]), uvBottomImage(&global_image[0]),
        yTopImage(&global_image[0]), uvTopImage(&global_image[0]),
        colorBottomImage(reinterpret_cast<uint8_t*>(&global_image[0])),
        colorTopImage(reinterpret_cast<uint8_t*>(&global_image[0])),
        visionSensorsProvider(&Sensors::updateVisionSensorsInMemory, this, mVisionSensors),
        motionSensorsProvider(&Sensors::updateMotionSensorsInMemory, this, mMotionSensors),
        varianceMonitor(MONITOR_COUNT, "SensorVariance", sensorNames),
        fsrMonitor(BUMPER_LEFT_L, "FSR_Variance", fsrNames),
        unfilteredInertial(), chestButton(0.0f), batteryCharge(0.0f),
        batteryCurrent(0.0f), FRM_FOLDER("/home/nao/nbites/frames") {

    mutex* vision_sensors_mutices[] = { &battery_mutex, &bumper_mutex,
            &ultra_sound_mutex };
    vision_sensors_mutex = multi_mutex(vision_sensors_mutices, "vision_sensors");
    mutex* motion_sensors_mutices[] = { &button_mutex, &fsr_mutex,
            &inertial_mutex, &unfiltered_inertial_mutex };
    motion_sensors_mutex = multi_mutex(motion_sensors_mutices, "motion_sensors");

    // set up the sensor monitoring
    varianceMonitor.Sensor(ACCX).setVarianceBounds(
            SensorMonitor::DONT_CHECK, ACCELEROMETER_HIGH_XY);
    varianceMonitor.Sensor(ACCY).setVarianceBounds(
            SensorMonitor::DONT_CHECK, ACCELEROMETER_HIGH_XY);
    varianceMonitor.Sensor(ACCZ).setVarianceBounds(
            SensorMonitor::DONT_CHECK, ACCELEROMETER_HIGH_Z);
    varianceMonitor.Sensor(GYROX).setVarianceBounds(
            GYRO_LOW, SensorMonitor::DONT_CHECK);
    varianceMonitor.Sensor(GYROY).setVarianceBounds(
            GYRO_LOW, SensorMonitor::DONT_CHECK);
    varianceMonitor.Sensor(ANGLEX).setVarianceBounds(
            SensorMonitor::DONT_CHECK, ANGLE_XY_HIGH);
    varianceMonitor.Sensor(ANGLEY).setVarianceBounds(
            SensorMonitor::DONT_CHECK, ANGLE_XY_HIGH);
    varianceMonitor.Sensor(SONARL).setVarianceBounds(
            SensorMonitor::DONT_CHECK, SONAR_HIGH);
    varianceMonitor.Sensor(SONARR).setVarianceBounds(
            SensorMonitor::DONT_CHECK, SONAR_HIGH);

    // all FSRs have the same variance range
    for (int i = 0; i <= FSR_RIGHT_B_R; ++i) {
        fsrMonitor.Sensor(i).setVarianceBounds(SensorMonitor::DONT_CHECK,
                FSR_HIGH);
    }

    // give the variance monitors access to speech
    varianceMonitor.SpeechPointer(speech);
    // talking robots are annoying
    //fsrMonitor.SpeechPointer(speech);

    // THIS IS AN OCTAL NUMBER, must start with 0
    mkdir(FRM_FOLDER.c_str(), 0755); // permissions: u+rwx, og+rx
}

Sensors::~Sensors() {
}

const vector<float> Sensors::getBodyAngles() const {
    angles_mutex.lock();

    vector<float> vec(bodyAngles);

    angles_mutex.unlock();

    return vec;
}

float Sensors::getBodyAngle(Kinematics::JointName joint) const {
    angles_mutex.lock();

    const float angle = bodyAngles[static_cast<int>(joint)];

    angles_mutex.unlock();
    return angle;
}

const vector<float> Sensors::getBodyAngles_degs() const {
    angles_mutex.lock();

    vector<float> vec(bodyAngles);

    angles_mutex.unlock();

    // Convert the angles from radians to degrees
    std::for_each(vec.begin(), vec.end(), _1 = _1 * TO_DEG);

    return vec;
}

const vector<float> Sensors::getVisionBodyAngles() const {
    vision_angles_mutex.lock();

    vector<float> vec(visionBodyAngles);

    vision_angles_mutex.unlock();

    return vec;
}

float Sensors::getVisionAngle(Kinematics::JointName joint) const {

    vision_angles_mutex.lock();

    float angle = visionBodyAngles[static_cast<int>(joint)];

    vision_angles_mutex.unlock();

    return angle;
}

const vector<float> Sensors::getMotionBodyAngles_degs() const {
    motion_angles_mutex.lock();

    vector<float> vec(motionBodyAngles);

    motion_angles_mutex.unlock();;

    // Convert the angles from radians to degrees
    std::for_each(vec.begin(), vec.end(), _1 = _1 * TO_DEG);

    return vec;
}

const vector<float> Sensors::getMotionBodyAngles() const {
    motion_angles_mutex.lock();

    vector<float> vec(motionBodyAngles);

    motion_angles_mutex.unlock();;

    return vec;
}

const vector<float> Sensors::getBodyTemperatures() const {
    temperatures_mutex.lock();

    vector<float> vec(bodyTemperatures);

    temperatures_mutex.unlock();;

    return vec;
}

const vector<float> Sensors::getBodyAngleErrors() const {
    errors_mutex.lock();

    vector<float> vec(bodyAnglesError);

    errors_mutex.unlock();;

    return vec;
}

float Sensors::getBodyAngleError(Kinematics::JointName joint) const {
    errors_mutex.lock();

    const float angleError = bodyAnglesError[static_cast<int>(joint)];

    errors_mutex.unlock();;

    return angleError;
}

const FSR Sensors::getLeftFootFSR() const {
    fsr_mutex.lock();

    const FSR left(leftFootFSR);

    fsr_mutex.unlock();;

    return left;
}

const FSR Sensors::getRightFootFSR() const {
    fsr_mutex.lock();

    const FSR right(rightFootFSR);

    fsr_mutex.unlock();;

    return right;
}

const FootBumper Sensors::getLeftFootBumper() const {
    bumper_mutex.lock();

    const FootBumper bumper = leftFootBumper;

    bumper_mutex.unlock();;

    return bumper;
}

const FootBumper Sensors::getRightFootBumper() const {
    bumper_mutex.lock();

    const FootBumper bumper = rightFootBumper;

    bumper_mutex.unlock();;

    return bumper;
}

const Inertial Sensors::getInertial() const {
    inertial_mutex.lock();

    const Inertial inert(inertial);

    inertial_mutex.unlock();;

    return inert;
}

const Inertial Sensors::getInertial_degs() const {
    inertial_mutex.lock();

    Inertial inert(inertial);

    inertial_mutex.unlock();;

    inert.angleX *= TO_DEG;
    inert.angleY *= TO_DEG;

    return inert;
}

const Inertial Sensors::getUnfilteredInertial() const {
    unfiltered_inertial_mutex.lock();

    const Inertial inert(unfilteredInertial);

    unfiltered_inertial_mutex.unlock();;

    return inert;
}

const float Sensors::getUltraSoundLeft() const {
    ultra_sound_mutex.lock();

    float dist = ultraSoundDistanceLeft;

    ultra_sound_mutex.unlock();

    return dist;
}

const float Sensors::getUltraSoundRight() const {
    ultra_sound_mutex.lock();

    float dist = ultraSoundDistanceRight;

    ultra_sound_mutex.unlock();

    return dist;
}

const float Sensors::getUltraSoundLeft_cm() const {
    ultra_sound_mutex.lock();

    float dist = ultraSoundDistanceLeft;

    ultra_sound_mutex.unlock();

    return dist * M_TO_CM;
}

const float Sensors::getUltraSoundRight_cm() const {
    ultra_sound_mutex.lock();

    float dist = ultraSoundDistanceRight;

    ultra_sound_mutex.unlock();

    return dist * M_TO_CM;
}

const SupportFoot Sensors::getSupportFoot() const {
    support_foot_mutex.lock();

    SupportFoot foot = supportFootHistory.back();

    support_foot_mutex.unlock();

    return foot;
}

const float Sensors::getChestButton() const {
    button_mutex.lock();

    float button = chestButton;

    button_mutex.unlock();;

    return button;
}

const float Sensors::getBatteryCharge() const {
    battery_mutex.lock();

    const float charge = batteryCharge;

    battery_mutex.unlock();;

    return charge;
}
const float Sensors::getBatteryCurrent() const {
    battery_mutex.lock();

    const float current = batteryCurrent;

    battery_mutex.unlock();;

    return current;
}

void Sensors::setBodyAngles(float* jointVPointers[]) {
    angles_mutex.lock();

    for (uint i = 0; i < Kinematics::NUM_JOINTS; i++) {
    	bodyAngles[i] = *jointVPointers[i];
    }
    /*
     cout << "Body angles in sensors";
     for (int i = 0 ; i < 22; i++){
     cout <<  bodyAngles[i] << " ";

     }
     cout << endl;
     */

    bodyAngleHistory.push_back(bodyAngles);

    if (bodyAngleHistory.size() > 10) {
        bodyAngleHistory.pop_front();
    }

    angles_mutex.unlock();
}

void Sensors::setVisionBodyAngles(const vector<float>& v) {
    vision_angles_mutex.lock();

    visionBodyAngles = v;

    vision_angles_mutex.unlock();
}

void Sensors::setMotionBodyAngles(const vector<float>& v) {
    motion_angles_mutex.lock();

    motionBodyAngles = v;

    motion_angles_mutex.unlock();;
}

void Sensors::setBodyAngleErrors(const vector<float>& v) {
    errors_mutex.lock();

    bodyAnglesError = v;

    errors_mutex.unlock();;
}

void Sensors::setBodyTemperatures(float* jointTPointers[]) {
    temperatures_mutex.lock();

    for (uint i = 0; i < Kinematics::NUM_JOINTS; i++) {
    	bodyTemperatures[i] = *jointTPointers[i];
    }

    temperatures_mutex.unlock();;
}

void Sensors::setLeftFootFSR(const float frontLeft, const float frontRight,
        const float rearLeft, const float rearRight) {
    fsr_mutex.lock();

    leftFootFSR = FSR(frontLeft, frontRight, rearLeft, rearRight);

    fsr_mutex.unlock();;
}

void Sensors::setRightFootFSR(const float frontLeft, const float frontRight,
        const float rearLeft, const float rearRight) {
    fsr_mutex.lock();

    rightFootFSR = FSR(frontLeft, frontRight, rearLeft, rearRight);

    fsr_mutex.unlock();;
}

void Sensors::setFSR(const FSR &_leftFootFSR, const FSR &_rightFootFSR) {
    fsr_mutex.lock();

    leftFootFSR = _leftFootFSR;
    rightFootFSR = _rightFootFSR;

    fsr_mutex.unlock();;
}

void Sensors::setLeftFootBumper(const float left, const float right) {
    bumper_mutex.lock();

    leftFootBumper = FootBumper(left, right);

    bumper_mutex.unlock();;
}

void Sensors::setLeftFootBumper(const FootBumper& bumper) {
    bumper_mutex.lock();

    leftFootBumper = bumper;

    bumper_mutex.unlock();;
}

void Sensors::setRightFootBumper(const float left, const float right) {
    bumper_mutex.lock();

    rightFootBumper = FootBumper(left, right);

    bumper_mutex.unlock();;
}

void Sensors::setRightFootBumper(const FootBumper& bumper) {
    bumper_mutex.lock();

    rightFootBumper = bumper;

    bumper_mutex.unlock();;
}

void Sensors::setInertial(const float accX, const float accY, const float accZ,
        const float gyrX, const float gyrY, const float angleX,
        const float angleY) {
    inertial_mutex.lock();

    inertial = Inertial(accX, accY, accZ, gyrX, gyrY, angleX, angleY);

    inertial_mutex.unlock();;
}

void Sensors::setInertial(const Inertial &v) {
    inertial_mutex.lock();

    inertial = v;

    inertial_mutex.unlock();;
}

void Sensors::setUnfilteredInertial(const float accX, const float accY,
        const float accZ, const float gyrX, const float gyrY,
        const float angleX, const float angleY) {
    unfiltered_inertial_mutex.lock();

    unfilteredInertial = Inertial(accX, accY, accZ, gyrX, gyrY, angleX, angleY);

    updateMotionDataVariance();

    unfiltered_inertial_mutex.unlock();;
}

void Sensors::setUnfilteredInertial(const Inertial &v) {
    unfiltered_inertial_mutex.lock();

    unfilteredInertial = v;

    updateMotionDataVariance();

    unfiltered_inertial_mutex.unlock();;
}

void Sensors::setUltraSound(const float distLeft, const float distRight) {
    ultra_sound_mutex.lock();

    ultraSoundDistanceLeft = distLeft;
    ultraSoundDistanceRight = distRight;

    updateVisionDataVariance();

    ultra_sound_mutex.unlock();
}

void Sensors::setSupportFoot(const SupportFoot _supportFoot) {
    support_foot_mutex.lock();

    supportFootHistory.push_back(_supportFoot);

    if (supportFootHistory.size() > 10) {
        supportFootHistory.pop_front();
    }

    support_foot_mutex.unlock();
}

/**
 * Sets the sensors which are updated on the motion frequency (every 20ms)
 */
void Sensors::setMotionSensors(const FSR &_leftFoot, const FSR &_rightFoot,
        const float _chestButton, const Inertial &_inertial,
        const Inertial & _unfilteredInertial) {
    motion_sensors_mutex.lock();

    leftFootFSR = _leftFoot;
    rightFootFSR = _rightFoot;
    chestButton = _chestButton;
    inertial = _inertial;
    unfilteredInertial = _unfilteredInertial;

    updateMotionDataVariance();

    motion_sensors_mutex.unlock();

    motionSensorsProvider.updateMemory();
}

/**
 * Sets the sensors which are updated on the vision frequency (every ?? ms)
 */
void Sensors::setVisionSensors(const FootBumper &_leftBumper,
        const FootBumper &_rightBumper, const float ultraSoundLeft,
        const float ultraSoundRight, const float bCharge,
        const float bCurrent) {
    vision_sensors_mutex.lock();

    leftFootBumper = _leftBumper;
    rightFootBumper = _rightBumper;
    ultraSoundDistanceLeft = ultraSoundLeft;
    ultraSoundDistanceRight = ultraSoundRight;
    batteryCharge = bCharge;
    batteryCurrent = bCurrent;

    updateVisionDataVariance();

    vision_sensors_mutex.unlock();
    visionSensorsProvider.updateMemory();
}

void Sensors::setBatteryCharge(float charge) {
    battery_mutex.lock();
    batteryCharge = charge;
    battery_mutex.unlock();;
}
void Sensors::setBatteryCurrent(float current) {
    battery_mutex.lock();
    batteryCurrent = current;
    battery_mutex.unlock();;
}

void Sensors::lockImage() const {
    image_mutex.lock();
}

void Sensors::releaseImage() const {
    image_mutex.unlock();
}

/**
 * Beware!!! I am toying with the possibility that a possible deadlock condition
 * exists here. If we somehow are able to lock body_angles, but unable to lock
 * vision_body_angles because someone else is using it, but that someone else
 * is also waiting on the body_angles mutex, we get a beautiful deadlock. I don't
 * think anyone else requires both mutexes locked at the same time. Beware
 * nonetheless.
 */
//TODO: (octavian) rename this and clean up the history part
// We keep a history of the past 10 angles to get the joints to sync
// up to vision properly
void Sensors::updateVisionAngles(int historyIndex) {
    angles_mutex.lock();
    vision_angles_mutex.lock();

    int i = 0;

    AngleHistory::reverse_iterator it =  bodyAngleHistory.rbegin();
    SupportFootHistory::reverse_iterator foot_it = supportFootHistory.rbegin();

    //look back in history historyIndex number of joint sets
    while (it != bodyAngleHistory.rend() && i != historyIndex) {
        it++; i++; foot_it++;
    }
    visionBodyAngles = *it;
    visionSupportFoot = *foot_it;

    vision_angles_mutex.unlock();
    angles_mutex.unlock();
}

const uint16_t* Sensors::getYImage(Camera::Type which) const {
    if(which == Camera::BOTTOM) return yBottomImage;
    else return yTopImage;
}

const uint16_t* Sensors::getImage(Camera::Type which) const {
    if(which == Camera::BOTTOM) return yBottomImage;
    else return yTopImage;
}

const uint16_t* Sensors::getUVImage(Camera::Type which) const {
    if(which == Camera::BOTTOM) return uvBottomImage;
    else return uvTopImage;
}

const uint8_t* Sensors::getColorImage(Camera::Type which) const {
    if(which == Camera::BOTTOM) return colorBottomImage;
    else return colorTopImage;
}

void Sensors::setImage(const uint16_t *img, Camera::Type which) {
    if(which == Camera::BOTTOM)
    {
        yBottomImage = img;
        uvBottomImage = img + AVERAGED_IMAGE_SIZE;
        colorBottomImage = reinterpret_cast<const uint8_t*>(img
                                      + AVERAGED_IMAGE_SIZE * 3);
    }
    else
    {
        yTopImage = img;
        uvTopImage = img + AVERAGED_IMAGE_SIZE;
        colorTopImage = reinterpret_cast<const uint8_t*>(img
                                   + AVERAGED_IMAGE_SIZE * 3);
    }
}

void Sensors::updateVisionSensorsInMemory(man::memory::MVisionSensors::ptr vs) const {

    using namespace man::memory::proto;

    vs->get()->clear_vision_body_angles();
    vector<float> bodyAngles = this->getVisionBodyAngles();
    for (vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end();
            i++) {
        vs->get()->add_vision_body_angles(*i);
    }

    FootBumper leftFootBumper = this->getLeftFootBumper();
    PSensors::PFootBumper* lfb = vs->get()->mutable_left_foot_bumper();
    lfb->set_left(leftFootBumper.left);
    lfb->set_right(leftFootBumper.right);
    FootBumper rightFootBumper = this->getRightFootBumper();
    PSensors::PFootBumper* rfb = vs->get()->mutable_right_foot_bumper();
    rfb->set_left(rightFootBumper.left);
    rfb->set_right(rightFootBumper.right);

    vs->get()->set_ultra_sound_distance_left(this->getUltraSoundLeft());
    vs->get()->set_ultra_sound_distance_right(this->getUltraSoundRight());

    vs->get()->set_battery_charge(this->getBatteryCharge());
    vs->get()->set_battery_current(this->getBatteryCurrent());
}

void Sensors::updateMotionSensorsInMemory(man::memory::MMotionSensors::ptr ms) const {

    using namespace man::memory::proto;

    ms->get()->clear_body_angles();
    vector<float> bodyAngles = this->getBodyAngles();
    for (vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end(); i++) {
        ms->get()->add_body_angles(*i);
    }

    ms->get()->clear_body_temperatures();
    vector<float> bodyTemperatures = this->getBodyTemperatures();
    for (vector<float>::iterator i = bodyTemperatures.begin(); i != bodyTemperatures.end(); i++) {
        ms->get()->add_body_temperatures(*i);
    }

    const Inertial _inertial = this->getInertial();
    PSensors::PInertial* inertial = ms->get()->mutable_inertial();
    inertial->set_acc_x(_inertial.accX);
    inertial->set_acc_y(_inertial.accY);
    inertial->set_acc_z(_inertial.accZ);
    inertial->set_gyr_x(_inertial.accX);
    inertial->set_gyr_y(_inertial.accY);
    inertial->set_angle_x(_inertial.angleX);
    inertial->set_angle_y(_inertial.angleY);

    const FSR _lfsr = this->getLeftFootFSR();
    PSensors::PFSR* lfsr = ms->get()->mutable_left_foot_fsr();
    lfsr->set_front_left(_lfsr.frontLeft);
    lfsr->set_front_right(_lfsr.frontRight);
    lfsr->set_rear_left(_lfsr.rearLeft);
    lfsr->set_rear_right(_lfsr.rearRight);
    const FSR _rfsr = this->getRightFootFSR();
    PSensors::PFSR* rfsr = ms->get()->mutable_right_foot_fsr();
    rfsr->set_front_left(_rfsr.frontLeft);
    rfsr->set_front_right(_rfsr.frontRight);
    rfsr->set_rear_left(_rfsr.rearLeft);
    rfsr->set_rear_right(_rfsr.rearRight);

    ms->get()->set_support_foot(this->getSupportFoot());
}

// The version for the frame format
static const int VERSION = 0;

void Sensors::updateMotionDataVariance() {
    variance_mutex.lock();

    int i = 0; // so re-ordering of sensors is easy

    // Inertial stuff
    varianceMonitor.update(i, unfilteredInertial.accX);
    varianceMonitor.update(++i, unfilteredInertial.accY);
    varianceMonitor.update(++i, unfilteredInertial.accZ);
    varianceMonitor.update(++i, unfilteredInertial.gyrX);
    varianceMonitor.update(++i, unfilteredInertial.gyrY);
    varianceMonitor.update(++i, unfilteredInertial.angleX);
    varianceMonitor.update(++i, unfilteredInertial.angleY);

    // FSRs (in their own monitor)
    i = 0;

    fsrMonitor.update(i, leftFootFSR.frontLeft);
    fsrMonitor.update(++i, leftFootFSR.frontRight);
    fsrMonitor.update(++i, leftFootFSR.rearLeft);
    fsrMonitor.update(++i, leftFootFSR.rearRight);
    fsrMonitor.update(++i, rightFootFSR.frontLeft);
    fsrMonitor.update(++i, rightFootFSR.frontRight);
    fsrMonitor.update(++i, rightFootFSR.rearLeft);
    fsrMonitor.update(++i, rightFootFSR.rearRight);

    variance_mutex.unlock();
}

void Sensors::updateVisionDataVariance() {
    variance_mutex.lock();

    int i = 7; /// @see updateMotionDataVariance()

    varianceMonitor.update(i, ultraSoundDistanceLeft);
    varianceMonitor.update(++i, ultraSoundDistanceRight);

    variance_mutex.unlock();
}

// tells our sensor monitors to dump their data to /tmp/
void Sensors::writeVarianceData() {
    variance_mutex.lock();

    cout << "Logging variance data to /tmp/" << endl;
    varianceMonitor.LogOutput();
    fsrMonitor.LogOutput();

    variance_mutex.unlock();
}

/**
 * These methods provide access to a percentage of each sensor type
 * that is broken, so we can decide in behaviors whether or not to
 * use that sensor.
 *
 * @return count of number of sensors that have reported an error
 */

float Sensors::percentBrokenFSR() {
    int brokenFSRs = 0;
    int numberFSRs = fsrMonitor.NumberMonitors();

    for (int i = 0; i < numberFSRs; ++i)
        if (!fsrMonitor.Sensor(i).isTrustworthy())
            ++brokenFSRs;

    return static_cast<float>(brokenFSRs) / static_cast<float>(numberFSRs);
}

float Sensors::percentBrokenMotionSensors() {
    int brokenMotion = 0;
    // sonar is checked elsewhere, so don't double count
    int motionSensors = varianceMonitor.NumberMonitors() - 2;

    for (int i = 0; i < motionSensors; ++i)
        if (!varianceMonitor.Sensor(i).isTrustworthy())
            ++brokenMotion;

    return static_cast<float>(brokenMotion) / static_cast<float>(motionSensors);
}

bool Sensors::angleXYBroken() {
    return !(varianceMonitor.Sensor(ANGLEX).isTrustworthy()
            && varianceMonitor.Sensor(ANGLEY).isTrustworthy());
}

float Sensors::percentBrokenSonar() {
    int brokenSonars = 0;

    if (!varianceMonitor.Sensor(SONARL).isTrustworthy())
        ++brokenSonars;

    if (!varianceMonitor.Sensor(SONARR).isTrustworthy())
        ++brokenSonars;

    return static_cast<float>(brokenSonars) * 0.5f; // only two Sonar sensors
}
