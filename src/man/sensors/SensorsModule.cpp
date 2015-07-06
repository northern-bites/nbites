#include "SensorsModule.h"
#include "Common.h"
#include "HighResTimer.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

namespace man {
namespace sensors {

SensorsModule::SensorsModule()
    : portals::Module(),
      jointsOutput_(base()),
      currentsOutput_(base()),
      temperatureOutput_(base()),
      chestboardButtonOutput_(base()),
      footbumperOutput_(base()),
      inertialsOutput_(base()),
      sonarsOutput_(base()),
      fsrOutput_(base()),
      batteryOutput_(base()),
      stiffStatusOutput_(base()),
      sitDownOutput_(base()),
      lastPrint(0),
      sitDown(0)
{
    shared_fd = shm_open(NBITES_MEM, O_RDWR, 0600);
    if (shared_fd < 0) {
        std::cout << "sensorsModule couldn't open shared fd!" << std::endl;
        exit(0);
    }

    shared = (volatile SharedData*) mmap(NULL, sizeof(SharedData),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        std::cout << "sensorsModule couldn't map to pointer!" << std::endl;
        exit(0);
    }
}

SensorsModule::~SensorsModule()
{
    // Close shared memory
    munmap((void *)shared, sizeof(SharedData));
    close(shared_fd);
}


// Helper method so that we can print out a Sweet Moves joint angle
// tuple directly when we want to (ie 5 button presses)
std::string SensorsModule::makeSweetMoveTuple(const messages::JointAngles* angles)
{
    char output[240];

    sprintf(output, "((%3.1f, %3.1f, %3.1f, %3.1f),\n(%3.1f, %3.1f, %3.1f, %3.1f, %3.1f, %3.1f),\n(%3.1f, %3.1f, %3.1f, %3.1f, %3.1f, %3.1f),\n(%3.1f, %3.1f, %3.1f, %3.1f))\n",
            TO_DEG*angles->l_shoulder_pitch(),
            TO_DEG*angles->l_shoulder_roll(),
            TO_DEG*angles->l_elbow_yaw(),
            TO_DEG*angles->l_elbow_roll(),
            TO_DEG*angles->l_hip_yaw_pitch(),
            TO_DEG*angles->l_hip_roll(),
            TO_DEG*angles->l_hip_pitch(),
            TO_DEG*angles->l_knee_pitch(),
            TO_DEG*angles->l_ankle_pitch(),
            TO_DEG*angles->l_ankle_roll(),
            TO_DEG*angles->l_hip_yaw_pitch(),
            TO_DEG*angles->r_hip_roll(),
            TO_DEG*angles->r_hip_pitch(),
            TO_DEG*angles->r_knee_pitch(),
            TO_DEG*angles->r_ankle_pitch(),
            TO_DEG*angles->r_ankle_roll(),
            TO_DEG*angles->r_shoulder_pitch(),
            TO_DEG*angles->r_shoulder_roll(),
            TO_DEG*angles->r_elbow_yaw(),
            TO_DEG*angles->r_elbow_roll());

    return std::string(output);
}
    
bool sensorSyncRead(volatile SharedData * sd, uint8_t * stage)
{
    pthread_mutex_t * lock = (pthread_mutex_t *) &sd->sensor_mutex;
    
    pthread_mutex_lock(lock);
    memcpy(stage,(void *) sd->sensors, SENSOR_SIZE);
    pthread_mutex_unlock(lock);
    
    return true;
}

void SensorsModule::updateSensorValues()
{
    std::string jointsS;
    std::string currentsS;
    std::string tempsS;
    std::string chestButtonS;
    std::string footBumperS;
    std::string inertialsS;
    std::string sonarsS;
    std::string fsrS;
    std::string batteryS;
    std::string stiffStatusS;
    
    if (!sensorSyncRead(shared, sensorsStage)) {
        printf("SensorsModule::updateSensorValues could not sensorSyncRead()\n");
        return;
    }
    

    Deserialize des(sensorsStage);
    if (!des.parse() || des.nObjects() < 10) {
        std::cout << "Sensors couldn't parse anything from shared memory! returning" << std::endl;
        return;
    }

    jointsS = des.stringNext();
    currentsS = des.stringNext();
    tempsS = des.stringNext();
    chestButtonS = des.stringNext();
    footBumperS = des.stringNext();
    inertialsS = des.stringNext();
    sonarsS = des.stringNext();
    fsrS = des.stringNext();
    batteryS = des.stringNext();
    stiffStatusS = des.string();

    shared->latestSensorRead = des.dataIndex();

    values.joints.ParseFromString(jointsS);
    values.currents.ParseFromString(currentsS);
    values.temperature.ParseFromString(tempsS);
    values.chestButton.ParseFromString(chestButtonS);
    values.footBumper.ParseFromString(footBumperS);
    values.inertials.ParseFromString(inertialsS);
    values.sonars.ParseFromString(sonarsS);
    values.fsr.ParseFromString(fsrS);
    values.battery.ParseFromString(batteryS);
    values.stiffStatus.ParseFromString(stiffStatusS);

    portals::Message<messages::JointAngles> joints(&(values.joints));
    portals::Message<messages::JointAngles> currents(&(values.currents));
    portals::Message<messages::JointAngles> temps(&(values.temperature));
    portals::Message<messages::ButtonState> chestButton(&(values.chestButton));
    portals::Message<messages::FootBumperState> footBumper(&(values.footBumper));
    portals::Message<messages::InertialState> inertials(&(values.inertials));
    portals::Message<messages::SonarState> sonars(&(values.sonars));
    portals::Message<messages::FSR> fsrs(&(values.fsr));
    portals::Message<messages::BatteryState> battery(&(values.battery));
    portals::Message<messages::StiffStatus> stiffness(&(values.stiffStatus));

    portals::Message<messages::Toggle> sit(0);
    sit.get()->set_toggle(shared->sit);

    jointsOutput_.setMessage(joints);
    currentsOutput_.setMessage(currents);
    temperatureOutput_.setMessage(temps);
    chestboardButtonOutput_.setMessage(chestButton);
    footbumperOutput_.setMessage(footBumper);
    inertialsOutput_.setMessage(inertials);
    sonarsOutput_.setMessage(sonars);
    fsrOutput_.setMessage(fsrs);
    batteryOutput_.setMessage(battery);
    stiffStatusOutput_.setMessage(stiffness);
    sitDownOutput_.setMessage(sit);
}

void SensorsModule::run_()
{
    printInput.latch();

    // Simply update all sensor readings from shared memory
    updateSensorValues();

    if(printInput.message().toggle() != lastPrint)
    {
        std::cout << makeSweetMoveTuple(jointsOutput_.getMessage(true).get())
                  << std::endl;
        lastPrint = !lastPrint;
    }
}

} // namespace sensors
} // namespace man
