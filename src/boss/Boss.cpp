#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"
#include "JointNames.h"

#include <iostream>
#include <string.h>
#include <cstdlib>
#include <chrono>

#define MAN_RESTART 'r'
#define MAN_KILL    'k'
#define MAN_START   's'

/*
 Begin timing macros for debugging callbacks.
 define DCM_TIMING_DEBUG 0
    to hear when callbacks take > 10ms
 define DCM_TIMING_DEBUG 1
    to always hear about callback timing
 don't define DCM_TIMING_DEBUG
    to never hear from or call any code.
 */
#define DCM_TIMING_DEBUG 0
//#define DCM_TIMING_DEBUG 1

#ifdef DCM_TIMING_DEBUG



std::chrono::time_point<std::chrono::high_resolution_clock> pre_start, post_start, temp_start;
void DCM_TIMING_DEBUG_PRE1() {
    pre_start = std::chrono::high_resolution_clock::now();
}

void DCM_TIMING_DEBUG_PRE2() {
    auto tpnow = std::chrono::high_resolution_clock::now();
    long micros = std::chrono::duration_cast<std::chrono::microseconds>(tpnow - pre_start).count();
    //if longer than 10 milliseconds, or DCM_TIMING_DEBUG > 0, shout.
    if (micros > 10000 || DCM_TIMING_DEBUG) {
        printf("WARNING: DCMPreCallback took %li microseconds!\n", micros);
    }
}

void DCM_TIMING_DEBUG_POST1() {
    post_start = std::chrono::high_resolution_clock::now();
}

void DCM_TIMING_DEBUG_POST2() {
    auto tpnow = std::chrono::high_resolution_clock::now();
    long micros = std::chrono::duration_cast<std::chrono::microseconds>(tpnow - post_start).count();
    
    if (micros > 10000 || DCM_TIMING_DEBUG) {
        printf("WARNING: DCMPostCallback took %li microseconds!\n", micros);
    }
}

void DCM_TIMING_DEBUG_START() {
    temp_start = std::chrono::high_resolution_clock::now();
}

bool DCM_TIMING_DEBUG_END() {
    auto tpnow = std::chrono::high_resolution_clock::now();
    long micros = std::chrono::duration_cast<std::chrono::microseconds>(tpnow - temp_start).count();
    
    if (micros > 10000 || DCM_TIMING_DEBUG) {
        printf("WARNING: DCM_TIMING_DEBUG measured %li microseconds!\n", micros);
        return true;
    }
    return false;
}
#else

#define DCM_TIMING_DEBUG_PRE1()
#define DCM_TIMING_DEBUG_PRE2()
#define DCM_TIMING_DEBUG_POST1()
#define DCM_TIMING_DEBUG_POST2()

#define DCM_TIMING_DEBUG_START()
#define DCM_TIMING_DEBUG_END() false

#endif


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
    killingMan(false),
    shared_fd(-1),
    shared(NULL),
    commandSkips(0),
    sensorSkips(0),
    fifo_fd(-1)
{
    std::cout << "Boss Constructor" << std::endl;

    if (constructSharedMem() != 1) {
        std::cout << "Couldn't construct shared mem, oh well!" << std::endl;
        return;
    }

    // Link up to the DCM loop
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

    // The FIFO that we're going to listen for terminal commands on
    fifo_fd = open("/home/nao/nbites/nbitesFIFO", O_RDONLY | O_NONBLOCK);
    if (fifo_fd <= 0) {
        std::cout << "FIFO ERROR" << std::endl;
        std::cout << "Boss will not be able to receive commands from terminal" << std::endl;
    }

    std::cout << "Boss Constructed successfully!" << std::endl;

    startMan();

    // This will not return.
    listener();
}

Boss::~Boss()
{
    std::cout << "Deconstructing Boss" << std::endl;
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
        if (killingMan) killMan();
        sleep(2);
    }
}

int Boss::startMan() {
    // TODO make extra sure man isn't running yet?
    // Man uses a lock file so it shouldn't be necessary..
    if (manRunning) {
        std::cout << "Man is already running. Will not start." << std::endl;
        return -1;
    }

    std::cout << "Starting man!" << std::endl;
    pid_t child = fork();
    if (child > 0) {
        manPID = child;
        manRunning = true;
        std::cout << "\n\n\n=================================================\n\n\n" << std::endl;
    }
    else if (child == 0) {
        //replace this child process with an instance of man.
        execl("/home/nao/nbites/lib/man", "", NULL);
        printf("CHILD PROCESS FAILED TO EXECL MAN!\n");
        //exit(1); Can't do this because Aldebaran overrides it
        int val = *((int *) 0); // FORCE SEG FAULT
    }
    else {
        std::cout << "COULD NOT DETACH MAN" << std::endl;
        manRunning = false;
        return -1;
    }
    return 1;
}

int Boss::killMan() {
    // TODO make sure man is actually running. Necessary?
    if (!manRunning) {
        std::cout << "BOSS: Man is not running. Cannot kill" << std::endl;
        return -1;
    }

    std::cout << "Man missed " << manMissedFrames << " frames while running.\n";
    std::cout << "Boss skipped " << sensorSkips << " and " << commandSkips << "commands" << std::endl;

    shared->sit = 1;
    // A bit longer than it takes to sit down
    sleep(7);

    kill(manPID, SIGTERM);
    manRunning = false;

    // Give man a bit to get itself together, kill isn't instantaneous
    sleep(5);

    // Clear the buffers
    memset((void*)shared->sensors[0], 0, SENSOR_SIZE);
    memset((void*)shared->sensors[1], 0, SENSOR_SIZE);
    memset((void*)shared->command, 0, COMMAND_SIZE);
    shared->latestCommandWritten = 0;
    shared->latestCommandRead = 0;
    shared->latestSensorWritten = 0;
    shared->latestSensorRead = 0;
    shared->sit = 0;
    commandSkips = 0;
    sensorSkips = 0;

    // Just in case we interrupted (man) in the middle of a critical section
    pthread_mutex_destroy((pthread_mutex_t *) &shared->sensor_mutex[0]);
    pthread_mutex_destroy((pthread_mutex_t *) &shared->sensor_mutex[0]);
    pthread_mutex_destroy((pthread_mutex_t *) &shared->sensor_mutex[0]);
    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex[0], NULL);
    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex[1], NULL);
    pthread_mutex_init( (pthread_mutex_t *) &shared->cmnd_mutex, NULL);

    killingMan = false;
    return 0; // TODO actually return something. Necessary?
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
    }
    if (ftruncate(shared_fd, sizeof(SharedData)) == -1) {
        std::cout << "Couldn't truncate shared mem" << std::endl;
        return -1;
    }
    shared = (volatile SharedData*) mmap(NULL, sizeof(SharedData),
                                         PROT_READ | PROT_WRITE,
                                         MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        std::cout << "Couldn't map shared mem to pointer" << std::endl;
        return -1;
    }

    // Make sure memory is in known state
    memset((void *) shared, 0, sizeof(SharedData));
    shared->sensorSwitch = 0;

    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex[0], NULL);
    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex[1], NULL);
    pthread_mutex_init( (pthread_mutex_t *) &shared->cmnd_mutex, NULL);

    return 1;
}


bool bossSyncRead(volatile SharedData * sd, uint8_t * stage) {
    //We know there exists new data in >sd<,
    //now we just need to safely read it out.

    // trylock because we're in the DCMs cycle right now. We don't want to block!
    int lockret = pthread_mutex_trylock( (pthread_mutex_t *) &(sd->cmnd_mutex));
    if (lockret) {
        return false;
    }
    // Secured lock
    // Grab the data quickly then release
    memcpy(stage, (void *) sd->command, COMMAND_SIZE);
    pthread_mutex_unlock((pthread_mutex_t *) &(sd->cmnd_mutex));
    return true;
}

void Boss::DCMPreProcessCallback()
{
    DCM_TIMING_DEBUG_PRE1();
    // Make sure that we ONLY enact legitimate commands from Man
    if (!manRunning || shared->latestCommandWritten == 0) {
        enactor.noStiff();
        led.noMan();
        shared->latestCommandRead = shared->latestCommandWritten;
        DCM_TIMING_DEBUG_PRE2();
        return;
    }

    std::string joints;
    std::string stiffs;
    std::string leds;

    if (shared->latestCommandWritten > shared->latestCommandRead) {
        if (bossSyncRead(shared, cmndStaging)) {
            Deserialize des(cmndStaging);


            if (!des.parse()) {
                // Couldn't parse anything from shared memory
                // Could imply bad things?
                DCM_TIMING_DEBUG_PRE2();
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

            // Now pass mans commands to the DCM
            enactor.command(results.jointsCommand, results.stiffnessCommand);
            led.setLeds(ledResults);

        } else {
            printf("Boss::DCMPreProcessCallback COULD NOT READ FRESH COMMAND (skip)\n");
            ++commandSkips;
        }
    } else {
        //No new data to read.
        // ...
    }
    DCM_TIMING_DEBUG_PRE2();
}

bool bossSyncWrite(volatile SharedData * sd, uint8_t * stage, uint64_t index)
{
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
    DCM_TIMING_DEBUG_POST1();
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
    // Serialize the protobufs to shared mem
    if (!serializeTo(objects, nextSensorIndex, sensorStaging, SENSOR_SIZE, NULL)) {
        std::cout << "O HUCK! Couldn't serialize!" << std::endl;
        return;
    }
    if (!bossSyncWrite(shared, sensorStaging, nextSensorIndex)) {
        //printf("Boss::DCMPostProcessCallback COULD NOT POST FRESH SENSORS (skip)\n");
        ++sensorSkips;
    }
    uint64_t lastRead = shared->latestSensorRead;
    if (nextSensorIndex - lastRead > 2 && (lastRead != 0) && manRunning) {
        manMissedFrames++; // This (should) happen very rarely.
    }

    if (nextSensorIndex - lastRead > 100 && lastRead != 0) {
        // TODO: Kill? If we get here man is (most likely) already dead
        //std::cout << "Sensors aren't getting read! Did Man die?" << std::endl;
        //std::cout << "commandIndex: " << nextSensorIndex << " lastRead: " << lastRead << std::endl;
        //manRunning = false; // TODO
        // Man is running very slow
        killingMan = true;
    }
    DCM_TIMING_DEBUG_POST2();
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
