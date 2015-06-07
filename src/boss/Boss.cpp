#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"
#include "JointNames.h"

#include <iostream>

namespace boss {

Boss::Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name) :
    AL::ALModule(broker_, name),
    broker(broker_),
    dcm(broker->getDcmProxy()),//getSpecialisedProxy<AL::DCMProxy>("DCM")),
    sensor(broker),
    enactor(dcm),
    led(broker),
    manPID(-1),
    manRunning(false),
    shared(NULL)
{
    std::cout << "Boss Constructor" << std::endl;

    int err = constructSharedMem();
    if (err != 1) {
        std::cout << "Couldn't construct shared mem, oh well!" << std::endl;
        return;
    }

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

    startMan();
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
    std::cout << "Building man!" << std::endl;
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
    std::cout << "\t\t\tMan built!" << std::endl;
}

int Boss::killMan() {
    // TODO make sure man is actually running

    kill(manPID, SIGTERM);
    return 0; // TODO actually return something
}

int Boss::constructSharedMem()
{
    std::cout << "Constructing shared mem" << std::endl;
    shared_fd = shm_open(NBITES_MEM, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (shared_fd <= 0) {
        int err = errno;
        std::cout << "Couldn't open shared FD\n\tErrno: " << err << std::endl;
        if (err == EACCES) std::cout << "EACCES: " << EACCES << "\n";
        if (err == EEXIST) std::cout << "EEXIST: " << EEXIST << "\n";
        if (err == EINVAL) std::cout << "EINVAL: " << EINVAL << "\n";
        if (err == EMFILE) std::cout << "EMFILE: " << EMFILE << "\n";
        if (err == ENFILE) std::cout << "ENFILE: " << ENFILE << "\n";
        return -1;
        // TODO error
    }
    if (ftruncate(shared_fd, sizeof(SharedData)) == -1) {
        std::cout << "Couldn't truncate shared mem" << std::endl;
        return -1;
        // TODO error
    }
    shared = (SharedData*) mmap(NULL, sizeof(SharedData),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        std::cout << "Couldn't map shared mem to pointer" << std::endl;
        return -1;
        // TODO error
    }

    memset(shared, 0, sizeof(SharedData));

    shared->commandSwitch = -1;
    shared->sensorSwitch = -1;
    shared->commandReadIndex = 0;
    shared->sensorReadIndex = 0;

    //sharedMem->command = PTHREAD_MUTEX_INITALIZER;
    //sharedMem->sense = PTHREAD_MUTEX_INITALIZER;
    return 1;
}

void Boss::DCMPreProcessCallback()
{
    std::string joints;
    std::string stiffs;
    std::string leds;

    // Start sem here
    int index = shared->commandSwitch;
    uint64_t write = 0;//shared->commands[index].writeIndex;

    if (index != -1) {
        Deserialize des(shared->command[index]);
        des.parse();
        std::cout << des.nObjects() <<" objects in this many bytes: " << des.totalSize() << std::endl;
        joints = des.stringNext();
        std::cout << "joints" << std::endl;
        stiffs = des.stringNext();
        std::cout << "stiffs" << std::endl;
        leds = des.string();
        std::cout << "leds" << std::endl;
        shared->commandReadIndex = des.dataIndex();
        std::cout << "Read 'em" << std::endl;
    }
    // End sem
    if (index == -1) return;
    return;

    JointCommand results;
    results.jointsCommand.ParseFromString(joints);
    std::cout << results.jointsCommand.DebugString() << std::endl;
    results.stiffnessCommand.ParseFromString(stiffs);
    std::cout << results.stiffnessCommand.DebugString() << std::endl;
    messages::LedCommand ledResults;
    ledResults.ParseFromString(leds);
    std::cout << ledResults.DebugString() << std::endl;

    //enactor.command(angles.jointsCommand, angles.stiffnessCommand);
    //led.setLeds(leds);
}

void Boss::DCMPostProcessCallback()
{
    std::cout << "Post process!" << std::endl;
    SensorValues values = sensor.getSensors();

    std::vector<SerializableBase*> objects = {
        new ProtoSer(&values.joints),
        new ProtoSer(&values.currents),
        new ProtoSer(&values.temperature),
        new ProtoSer(&values.chestButton),
        new ProtoSer(&values.footBumper),
        new ProtoSer(&values.inertials),      // serializer deletes these
        new ProtoSer(&values.sonars),
        new ProtoSer(&values.fsr),
        new ProtoSer(&values.battery),
        new ProtoSer(&values.stiffStatus),
    };

    ++sensorIndex; // TODO
    // Start Semaphore here! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int index = shared->sensorSwitch ? 0 : 1;
    size_t usedSpace;
    bool returned = serializeTo(objects, sensorIndex, shared->sensors[index], SENSOR_SIZE, &usedSpace);
    shared->sensorSwitch = index;
    // End Semaphore here! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~``

    //std::cout << "We used this much space for sensors " << usedSpace << std::endl;
}
}