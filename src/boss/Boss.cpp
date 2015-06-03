#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"
#include "JointNames.h"

#include <assert.h>

#include <iostream>

#define assert(X) if(!(X)) std::cout << "Assertion " << #X << "did not pass" << std::endl;


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

    //startMan();
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

    //memset(shared, 0, sizeof(SharedData));

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

    // TODO grab semaphore
    int index = shared->sensorSwitch;
    if (index != -1)
    {
        Deserialize des(shared->sensors[index]);
        des.parse();

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

        sensorIndex = des.dataIndex();
        shared->sensorReadIndex = sensorIndex;
    }
    else {
        std::cout << "didn't grab because index -1" << std::endl;
    }
    // TODO Release semaphore

    if (index == -1) return;

    SensorValues values;

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
    // // //std::cout << "Preprocess!" << std::endl;

    // // std::string joints;
    // // std::string stiffs;
    // // std::string leds;

    // // // Start sem here
    // // int index = shared->commandSwitch;
    // // uint64_t write = 0;//shared->commands[index].writeIndex;

    // // if (index != -1) {
    // //     //std::cout << "Trying to access!!!" << std::endl;
    // //     Deserialize des(shared->command[index]);
    // //     des.parse();
    // //     joints = des.stringNext();
    // //     stiffs = des.stringNext();
    // //     leds = des.string();
    // // }
    // // // End sem
    // // if (index == -1) return;
    // // return;

    // // JointCommand results;
    // // results.jointsCommand.ParseFromString(joints);
    // // results.stiffnessCommand.ParseFromString(stiffs);
    // // messages::LedCommand ledResults;
    // // ledResults.ParseFromString(leds);

    //enactor.command(angles.jointsCommand, angles.stiffnessCommand);
    //led.setLeds(leds);
}

void assertValuesEqual()
{

}

void assertJointsEqual(messages::JointAngles one, messages::JointAngles two)
{
    assert(one.head_yaw() == two.head_yaw());
    assert(one.head_pitch() == two.head_pitch());
    assert(one.l_shoulder_pitch() == two.l_shoulder_pitch());
    assert(one.l_shoulder_roll() == two.l_shoulder_roll());
    assert(one.l_elbow_roll() == two.l_elbow_roll());
    assert(one.l_elbow_yaw() == two.l_elbow_yaw());
    assert(one.l_wrist_yaw() == two.l_wrist_yaw());
    assert(one.l_hand() == two.l_hand());
    assert(one.r_shoulder_pitch() == two.r_shoulder_pitch());
    assert(one.r_shoulder_roll() == two.r_shoulder_roll());
    assert(one.r_elbow_yaw() == two.r_elbow_yaw());
    assert(one.r_elbow_roll() == two.r_elbow_roll());
    assert(one.r_wrist_yaw() == two.r_wrist_yaw());
    assert(one.r_hand() == two.r_hand());
    assert(one.l_hip_yaw_pitch() == two.l_hip_yaw_pitch());
    assert(one.r_hip_yaw_pitch() == two.r_hip_yaw_pitch());
    assert(one.l_hip_roll() == two.l_hip_roll());
    assert(one.l_hip_pitch() == two.l_hip_pitch());
    assert(one.l_knee_pitch() == two.l_knee_pitch());
    assert(one.l_ankle_pitch() == two.l_ankle_pitch());
    assert(one.l_ankle_roll() == two.l_ankle_roll());
    assert(one.r_hip_roll() == two.r_hip_roll());
    assert(one.r_hip_pitch() == two.r_hip_pitch());
    assert(one.r_knee_pitch() == two.r_knee_pitch());
    assert(one.r_ankle_pitch() == two.r_ankle_pitch());
    assert(one.r_ankle_roll() == two.r_ankle_roll());
}

void assertButtonsEqual(messages::ButtonState one, messages::ButtonState two)
{

}

void assertBumpersEqual(messages::FootBumperState one, messages::FootBumperState two)
{

}

void assertFSREqual(messages::FSR one, messages::FSR two)
{

}

void assertStiffEqual(messages::StiffStatus one, messages::StiffStatus two)
{

}

        // The following is for deserializing sensor data
        // Deserialize deserialz(shared->stiffCommand[index], 1024);
        // const std::string joints = deserialz.stringNext();
        // const std::string currents = deserialz.stringNext();
        // const std::string temps = deserialz.stringNext();
        // const std::string chest = deserialz.stringNext();
        // const std::string footBump = deserialz.stringNext();
        // const std::string inertial = deserialz.stringNext();
        // const std::string sonars = deserialz.stringNext();
        // const std::string fsrs = deserialz.stringNext();
        // const std::string battery = deserialz.stringNext();
        // const std::string stiffStatus = deserialz.string();

void Boss::DCMPostProcessCallback()
{
    //std::cout << "Post process!" << std::endl;
    assert(shared);
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

    //std::cout << "We used this much space! " << usedSpace << std::endl;

    // std::string serial;
    // values.joints.SerializeToString(&serial);
    // std::cout << "JoinAngles serial is::: " << serial.size() << std::endl << std::endl << std::endl;
    // values.chestButton.SerializeToString(&serial);
    // std::cout << "chest serial is::: " << serial.size() << std::endl << std::endl << std::endl;
    // values.footBumper.SerializeToString(&serial);
    // std::cout << "footbump serial is::: " << serial.size() << std::endl << std::endl << std::endl;
    // values.inertials.SerializeToString(&serial);
    // std::cout << "inertials serial is::: " << serial.size() << std::endl << std::endl << std::endl;
    // values.sonars.SerializeToString(&serial);
    // std::cout << "sonars serial is::: " << serial.size() << std::endl << std::endl << std::endl;
    // values.fsr.SerializeToString(&serial);
    // std::cout << "FSR serial is::: " << serial.size() << std::endl << std::endl << std::endl;
    // values.battery.SerializeToString(&serial);
    // std::cout << "battery serial is::: " << serial.size() << std::endl << std::endl << std::endl;
    // values.stiffStatus.SerializeToString(&serial);
    // std::cout << "stiffstatus serial is::: " << serial.size() << std::endl << std::endl << std::endl;

    //std::cout << "POST l_shoulder_pitch " << values.joints.l_shoulder_pitch() << std::endl;
}

}
