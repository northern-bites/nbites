#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"
#include "JointNames.h"

#include <iostream>


namespace boss {

Boss::Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name) :
    AL::ALModule(broker_, name),
    broker(broker_),
    dcm(broker->getSpecialisedProxy<AL::DCMProxy>("DCM")),
    sensor(broker),
    enactor(dcm),
    manPID(-1),
    manRunning(false),
    sharedMem(NULL)
{
    std::cout << "Boss Constructor" << std::endl;

    initEnactor();
    initSensorAccess();

    try {
        dcmPreProcessConnection = broker_->getProxy("DCM")->getModule()->atPreProcess(
            boost::bind(&Boss::DCMPreProcessCallback, this));
    }
    catch(const AL::ALError& e) {
        std::cout << "Tried to bind preprocess, but failed, because " + e.toString() << std::endl;
    }
    try {
        dcmPostProcessConnection = broker_->getProxy("DCM")->getModule()->atPostProcess(
            boost::bind(&Boss::DCMPostProcessCallback, this));
    }
    catch(const AL::ALError& e) {
        std::cout << "Tried to bind postprocess, but failed, because " + e.toString() << std::endl;
    }


    std::cout << "Boss Constructed successfully!" << std::endl;
}

Boss::~Boss()
{
    std::cout << "Deconstructing" << std::endl;
    dcmPreProcessConnection.disconnect();
    dcmPostProcessConnection.disconnect();
}

int Boss::startMan() {
    // TODO make sure man isn't running yet
    pid_t child = fork();
    if (child > 0) {
        manPID = child;
        manRunning = true;
    }
    else if (child == 0) {
        execl("/home/nao/nbites/lib/man", "", NULL);
    }
    else {
        std::cout << "COULD NOT DETACH MAN" << std::endl;
        manRunning = false;
    }
}

int Boss::killMan() {
    // TODO make sure man is actually running

    kill(manPID, SIGTERM);
    return 0; // TODO actually return something
}

int Boss::constructSharedMem()
{
    int shared_fd = shm_open(MEMORY_NAME, O_RDWR | O_CREAT, 0600);
    if (shared_fd < 0) {
        // TODO error
    }
    if (ftruncate(shared_fd, sizeof(SharedData)) == -1) {
        // TODO error
    }

    SharedData* shared = (SharedData*) mmap(NULL, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        // TODO error
    }

    memset(shared, 0, sizeof(SharedData));

    sharedMem = shared;
}

void writeSensors()
{

    int i = 0;

    // if (sharedMem->sensorsRead != sharedMem->sensorsLatest) {
    //     // Man missed a frame of sensors
    //     // TODO notify?
    // }

}

void Boss::DCMPreProcessCallback()
{
    // Read in Joints to write from shared memory
    // Write them to the DCM
    // Needs to be FAST!
    messages::JointAngles angles;
    enactor.command(angles, angles);
}

void Boss::DCMPostProcessCallback()
{
    SensorValues values = sensor.getSensors();
    // Do something with the sensor values here!
    //std::cout << "l_shoulder_roll is: " << sensorValues[sensors::LElbowRoll] << std::endl;
}

}
