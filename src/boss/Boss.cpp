#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"
#include "JointNames.h"

#include <iostream>
#include <string.h>
#include <cstdlib>
#include <chrono>
#include <errno.h>

#define MAN_RESTART 'r'
#define MAN_KILL    'k'
#define MAN_START   's'
#define BOSS_PRINT  'p'

//#define DCM_DEBUG

#ifdef DCM_DEBUG
#define DCM_TIMING_DEBUG_ALWAYS false
std::chrono::time_point<std::chrono::high_resolution_clock> pre_start, post_start, temp_start;
void DCM_TIMING_DEBUG_PRE1() {
    pre_start = std::chrono::high_resolution_clock::now();
}

void DCM_TIMING_DEBUG_PRE2() {
    auto tpnow = std::chrono::high_resolution_clock::now();
    long micros = std::chrono::duration_cast<std::chrono::microseconds>(tpnow - pre_start).count();
    //if longer than 10 milliseconds, or DCM_TIMING_DEBUG > 0, shout.
    if (micros > 10000 || DCM_TIMING_DEBUG_ALWAYS) {
        printf("WARNING: DCMPreCallback took %li microseconds!\n", micros);
    }
}

void DCM_TIMING_DEBUG_POST1() {
    post_start = std::chrono::high_resolution_clock::now();
}

void DCM_TIMING_DEBUG_POST2() {
    auto tpnow = std::chrono::high_resolution_clock::now();
    long micros = std::chrono::duration_cast<std::chrono::microseconds>(tpnow - post_start).count();
    
    if (micros > 10000 || DCM_TIMING_DEBUG_ALWAYS) {
        printf("WARNING: DCMPostCallback took %li microseconds!\n", micros);
    }
}

void DCM_TIMING_DEBUG_START() {
    temp_start = std::chrono::high_resolution_clock::now();
}

bool DCM_TIMING_DEBUG_END() {
    auto tpnow = std::chrono::high_resolution_clock::now();
    long micros = std::chrono::duration_cast<std::chrono::microseconds>(tpnow - temp_start).count();
    
    if (micros > 10000 || DCM_TIMING_DEBUG_ALWAYS) {
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

#define MAN_DEAD_THRESHOLD 500
#define BOSS_MAIN_LOOP_US 500000

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
    cmndLockMiss(0),
    sensorLockMiss(0),
    fifo_fd(-1)
{
    printf("\t\tboss 7/6\n");
    std::cout << "Boss Constructor" << std::endl;

    if (constructSharedMem() != 1) {
        std::cout << "Couldn't construct shared mem, oh well!" << std::endl;
        return;
    }
    
    bool success = true;

    // Link up to the DCM loop
    try {
        dcmPreProcessConnection = broker_->getProxy("DCM")->getModule()->atPreProcess(
            boost::bind(&Boss::DCMPreProcessCallback, this));
    }
    catch(const AL::ALError& e) {
        std::cout << "Tried to bind preprocess, but failed, because " + e.toString() << std::endl;
        success = false;
    }
    try {
        dcmPostProcessConnection = broker_->getProxy("DCM")->getModule()->atPostProcess(
            boost::bind(&Boss::DCMPostProcessCallback, this));
    }
    catch(const AL::ALError& e) {
        std::cout << "Tried to bind postprocess, but failed, because " + e.toString() << std::endl;
        success = false;
    }

    // The FIFO that we're going to listen for terminal commands on
    fifo_fd = open("/home/nao/nbites/nbitesFIFO", O_RDONLY | O_NONBLOCK);
    if (fifo_fd <= 0) {
        std::cout << "FIFO ERROR" << std::endl;
        std::cout << "Boss will not be able to receive commands from terminal" << std::endl;
        success = false;
    }
    
    if ( pthread_mutexattr_init(&shared_mutex_attr) ||
        pthread_mutexattr_setpshared(&shared_mutex_attr, PTHREAD_PROCESS_SHARED) )
    {
        std::cout << "ERROR constructing shared process mutex attributes!" << std::endl;
        success = false;
    }
   
    if (success) {
        std::cout << "Boss Constructed successfully!" << std::endl;
    } else {
        std::cout << "\nWARNING: one or more errors while constructing Boss! Crash likely." << std::endl;
    }
    
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
    while(true)
    {
        if (manRunning) {
            
            if ( (shared->latestSensorWritten - shared->latestSensorRead) > MAN_DEAD_THRESHOLD ) {
                std::cout << "Boss::listener() killing man due to inactivity" << std::endl;
                print_info();
                killMan();
                continue;
            }
        }
        
        checkFIFO();
        
        usleep(BOSS_MAIN_LOOP_US);
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
        ::exit(1);
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

//    std::cout << "Man missed " << manMissedFrames << " frames while running.\n";
//    std::cout << "Boss skipped " << sensorLockMiss << " and " << cmndLockMiss << "commands" << std::endl;

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
    cmndLockMiss = 0;
    sensorLockMiss = 0;

    // Just in case we interrupted (man) in the middle of a critical section
    
    pthread_mutex_destroy((pthread_mutex_t *) &shared->sensor_mutex);
    pthread_mutex_destroy((pthread_mutex_t *) &shared->cmnd_mutex);
    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex, &shared_mutex_attr);
    pthread_mutex_init( (pthread_mutex_t *) &shared->cmnd_mutex, &shared_mutex_attr);

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

    pthread_mutex_init( (pthread_mutex_t *) &shared->sensor_mutex, &shared_mutex_attr);
    pthread_mutex_init( (pthread_mutex_t *) &shared->cmnd_mutex, &shared_mutex_attr);

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
            ++cmndLockMiss;
        }
    } else {
        //No new data to read.
        // ...
    }
    DCM_TIMING_DEBUG_PRE2();
}

bool bossSyncWrite(volatile SharedData * sd, uint8_t * stage, int64_t index)
{
    // trylock because we're in the DCMs cycle right now. We don't want to block!
    int lockret = pthread_mutex_trylock( (pthread_mutex_t *) &(sd->sensor_mutex));
    if (lockret) {
        return false;
    }
    // Secured lock
    // Grab the data quickly then release
    memcpy((void *)sd->sensors, stage, SENSOR_SIZE);
    sd->latestSensorWritten = index;
    pthread_mutex_unlock((pthread_mutex_t *) &(sd->sensor_mutex));
    
    return true;
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
    
    int64_t nextSensorIndex = (shared->latestSensorWritten + 1);
    // Serialize the protobufs to shared mem
    if (!serializeTo(objects, nextSensorIndex, sensorStaging, SENSOR_SIZE, NULL)) {
        std::cout << "O HUCK! Couldn't serialize!\n";
        DCM_TIMING_DEBUG_POST2();
        return;
    }
    
    if (!bossSyncWrite(shared, sensorStaging, nextSensorIndex)) {
        ++sensorLockMiss;
    }

    DCM_TIMING_DEBUG_POST2();
}

const int FIFO_CMND_SIZE = 2;
void Boss::checkFIFO() {
    // Command is going to be a single char, reading two characters consumes '\0'
    char command[FIFO_CMND_SIZE];

    ssize_t amt = read(fifo_fd, &command, FIFO_CMND_SIZE);
    
    if (amt < 0 && errno != EAGAIN) {
        int se = errno;
        printf("ERROR reading nbitesFIFO: %i\n", se);
        return;
    }
    
    switch (amt) {
        case 0:
            return;
        case 1:
            printf("Boss::checkFIFO ignoring partial read!\n");
            return;
        default:
            printf("Boss::checkFIFO got unexpected amt return: %li\n", amt);
            return;
        case FIFO_CMND_SIZE:
            ; //continue....
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
    case BOSS_PRINT:
        print_info();
        break;
    }
}
    
void Boss::print_info() {
    int64_t _lw = shared->latestSensorWritten;
    int64_t _lr = shared->latestSensorRead;
    printf("BOSS PRINT:\n\tpre-skips=%lld post-skips=%lld\n"
           "\tman-live: \t%lld\t[%lld - %lld]\n",
           cmndLockMiss,sensorLockMiss,
           _lw - _lr, _lw, _lr);
    
    fflush(stdout);
}
}
