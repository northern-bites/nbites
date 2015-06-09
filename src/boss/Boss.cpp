#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"
#include "JointNames.h"

#include <iostream>
#include <string.h>
#include <cstdlib>

#define MAN_RESTART 'r'
#define MAN_KILL    'k'
#define MAN_START   's'

namespace boss {

Boss::Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name) :
    AL::ALModule(broker_, name),
    broker(broker_),
    dcm(broker->getDcmProxy()),
    sensor(broker),
    enactor(dcm),
    led(broker),
    manPID(-1),
    manRunning(false),
    shared_fd(-1),
    shared(NULL),
    fifo_fd(-1)
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

    fifo_fd = open("/home/nao/nbites/nbitesFIFO", O_RDONLY | O_NONBLOCK);
    if (fifo_fd <= 0) {
        std::cout << "FIFO ERROR" << std::endl;
    }

    std::cout << "Boss Constructed successfully!" << std::endl;
    
    commandSkips = 0;
    sensorSkips = 0;
    
    startMan();

    // This will not return.
    listener();
}

Boss::~Boss()
{
    std::cout << "Deconstructing" << std::endl;
    dcmPreProcessConnection.disconnect();
    dcmPostProcessConnection.disconnect();

    // Close shared memory
    munmap((void *) shared, sizeof(SharedData));
    close(shared_fd);
    close(fifo_fd);
}

void Boss::listener()
{
    while(1)
    {
        checkFIFO();
        sleep(2);
    }
}

int Boss::startMan() {
    // TODO make sure man isn't running yet
    if (manRunning) {
        std::cout << "Man is already running. Will not start." << std::endl;
        return -1;
    }

    std::cout << "Building man!" << std::endl;
    pid_t child = fork();
    if (child > 0) {
        manPID = child;
        manRunning = true;
    }
    else if (child == 0) {
        //replace this child process with an instance of man.
        execl("/home/nao/nbites/lib/man", "", NULL);
        printf("CHILD PROCESS FAILED TO EXECL MAN!\n");
        //exit(1);
        int val = *((int *) 0); //FORCE SEG FAULT
    }
    else {
        std::cout << "COULD NOT DETACH MAN" << std::endl;
        manRunning = false;
        return -1;
    }
    std::cout << "\tMan built!" << std::endl;
    return 1;
}

int Boss::killMan() {
    // TODO make sure man is actually running
    if (!manRunning) {
        std::cout << "BOSS: Man is not running. Cannot kill" << std::endl;
        return -1;
    }

    kill(manPID, SIGTERM);
    manRunning = false;
    return 0; // TODO actually return something
}

int Boss::constructSharedMem()
{
    std::cout << "Constructing shared mem" << std::endl;
    shared_fd = shm_open(NBITES_MEM, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (shared_fd <= 0) {
        int err = errno;
        char buf[100];
        strerror_r(err, buf, 100);
        std::cout << "Couldn't open shared FD\n\tErrno: " << err << ": " << buf << std::endl;
        
        return -1;
        // TODO error
    }
    if (ftruncate(shared_fd, sizeof(SharedData)) == -1) {
        std::cout << "Couldn't truncate shared mem" << std::endl;
        return -1;
        // TODO error
    }
    shared = (volatile SharedData*) mmap(NULL, sizeof(SharedData),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        std::cout << "Couldn't map shared mem to pointer" << std::endl;
        return -1;
        // TODO error
    }

    memset((void *) shared, 0, sizeof(SharedData));
    shared->sensorSwitch = 0;
    
    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex[0], NULL);
    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex[1], NULL);
    pthread_mutex_init( (pthread_mutex_t *) &shared->cmnd_mutex, NULL);
    
    /*
    shared->sensor_mutex[0] = (volatile pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    shared->sensor_mutex[1] = (volatile pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    shared->cmnd_mutex = (volatile pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER; */
    return 1;
}
    
bool bossSyncRead(volatile SharedData * sd, uint8_t * stage) {
    //We know there exists new data in >sd<,
    //now we just need to safely read it out.
    //printf("br\n");
    
    int lockret = pthread_mutex_trylock( (pthread_mutex_t *) &(sd->cmnd_mutex));
    if (lockret) {
        return false;
    }
    //locked...
    
    memcpy(stage, (void *) sd->command, COMMAND_SIZE);
    pthread_mutex_unlock((pthread_mutex_t *) &(sd->cmnd_mutex));
    return true;
}

void Boss::DCMPreProcessCallback()
{
    if (!manRunning) return;

    std::string joints;
    std::string stiffs;
    std::string leds;
    
    if (shared->latestCommandWritten > shared->latestCommandRead) {
        if (bossSyncRead(shared, cmndStaging)) {
            Deserialize des(cmndStaging);
            
            if (!des.parse()) {
                return;
            }
            
            shared->latestCommandRead = des.dataIndex();

            joints = des.stringNext();
            stiffs = des.stringNext();
            leds = des.string();
            
            JointCommand results;
            results.jointsCommand.ParseFromString(joints);
            results.stiffnessCommand.ParseFromString(stiffs);
            messages::LedCommand ledResults;
            ledResults.ParseFromString(leds);
            
            //enactor.command(results.jointsCommand, results.stiffnessCommand);
            led.setLeds(ledResults);
            
        } else {
            printf("Boss::DCMPreProcessCallback COULD NOT READ FRESH COMMAND (skip)\n");
            ++commandSkips;
        }
    } else {
        //No new data to read.
        // ...
    }
}
            
bool bossSyncWrite(volatile SharedData * sd, uint8_t * stage, uint64_t index)
{
    //printf("bw\n");
    volatile uint8_t& newest = (sd->sensorSwitch);
    pthread_mutex_t * oldestLock = (pthread_mutex_t *) &sd->sensor_mutex[!(newest)];
    pthread_mutex_t * newestLock = (pthread_mutex_t *) &sd->sensor_mutex[ (newest)];

    if (
        pthread_mutex_trylock(oldestLock) == 0 //locked
        )
    {
        memcpy((void *)sd->sensors[!newest], stage, SENSOR_SIZE);
        newest = !newest;
        sd->latestSensorWritten = index;
        pthread_mutex_unlock(oldestLock);
        return true;
    } else if (
        pthread_mutex_trylock(newestLock) == 0
        ) {
        memcpy((void *)sd->sensors[newest], stage, SENSOR_SIZE);
        //newest = newest
        sd->latestSensorWritten = index;
        pthread_mutex_unlock(newestLock);
        return true;
    } else {
        return false;
    }
}

void Boss::DCMPostProcessCallback()
{
    if (!manRunning) return;

    SensorValues values = sensor.getSensors();

    std::vector<SerializableBase*> objects = {
        // serializer deletes these
        new ProtoSer(&values.joints),
        new ProtoSer(&values.currents),
        new ProtoSer(&values.temperature),
        new ProtoSer(&values.chestButton),
        new ProtoSer(&values.footBumper),
        new ProtoSer(&values.inertials),
        new ProtoSer(&values.sonars),
        new ProtoSer(&values.fsr),
        new ProtoSer(&values.battery),
        new ProtoSer(&values.stiffStatus),
    };
    uint64_t nextSensorIndex = (shared->latestSensorWritten + 1);
    if (!serializeTo(objects, nextSensorIndex, sensorStaging, SENSOR_SIZE, NULL)) {
        return;
    }
    
    if (!bossSyncWrite(shared, sensorStaging, nextSensorIndex)) {
        printf("Boss::DCMPostProcessCallback COULD NOT POST FRESH SENSORS (skip)\n");
        ++sensorSkips;
    }
    
    uint64_t lastRead = shared->latestSensorRead;
    if (nextSensorIndex - lastRead > 2) {
        std::cout << "MAN missed a frame" << std::endl;
    }

    if (nextSensorIndex - lastRead > 10) {
        std::cout << "Sensors aren't getting read! Did Man die?" << std::endl;
        //std::cout << "commandIndex: " << sensorIndex << " lastRead: " << lastRead << std::endl;
        //manRunning = false; // TODO
    }
}

void Boss::checkFIFO() {
    // Command is going to be a single char, reading two characters consumes '\0'
    char command[2];

    size_t amt = read(fifo_fd, &command, 2);

    if (amt == 0) {
        return; // Read nothing
    }

    switch(command[0]) {
    case MAN_RESTART:
        std::cout << "MAN_RESTART" << std::endl;
        killMan();
        startMan();
        break;
    case MAN_KILL:
        std::cout << "MAN_KILL" << std::endl;
        killMan();
        break;
    case MAN_START:
        std::cout << "MAN_START" << std::endl;
        startMan();
        break;
    }
}
}
