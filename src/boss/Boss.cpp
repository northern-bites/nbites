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
    led(broker),
    manPID(-1),
    manRunning(false),
    shared(NULL)
{
    std::cout << "Boss Constructor" << std::endl;

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

    constructSharedMem();

    std::cout << "Boss Constructed successfully!" << std::endl;
}

Boss::~Boss()
{
    std::cout << "Deconstructing" << std::endl;
    dcmPreProcessConnection.disconnect();
    dcmPostProcessConnection.disconnect();

    // Close shared memory
    munmap(shared, sizeof(SharedData));
    close(shared_fd);
    sem_close(semaphore);
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
    shared_fd = shm_open(NBITES_MEM, O_RDWR | O_CREAT, 0600);
    if (shared_fd < 0) {
        // TODO error
    }
    if (ftruncate(shared_fd, sizeof(SharedData)) == -1) {
        // TODO error
    }

    shared = (SharedData*) mmap(NULL, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        // TODO error
    }

    memset(shared, 0, sizeof(SharedData));

    //sharedMem->command = PTHREAD_MUTEX_INITALIZER;
    //sharedMem->sense = PTHREAD_MUTEX_INITALIZER;
}

void Boss::DCMPreProcessCallback()
{
    // Start sem here
    uint8_t index = shared->commandSwitch;
    JointCommand angles = shared->commands[index];
    messages::LedCommand leds = shared->leds[index];
    shared->commandReadIndex = shared->commands[index].writeIndex;
    // End sem

    enactor.command(angles.jointsCommand, angles.stiffnessCommand);
    led.setLeds(leds);
}

void Boss::DCMPostProcessCallback()
{
    SensorValues values = sensor.getSensors();
    values.writeIndex = ++sensorIndex;

    // Start Semaphore here!
    uint8_t index = shared->sensorReadIndex % 2;
    shared->sensors[index] = values;
    shared->sensorSwitch = index;
    // End Semaphore here!
}

}
