#include "JointEnactorModule.h"
#include "Profiler.h"
#include "HighResTimer.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "DebugConfig.h"

namespace man {
namespace jointenactor{

JointEnactorModule::JointEnactorModule() :
    commandIndex(0),
    lastRead(0)
{
    shared_fd = shm_open(NBITES_MEM, O_RDWR, 0600);
    if (shared_fd < 0) {
        std::cout << "Jointenactor couldn't open shared fd!" << std::endl;
        exit(0);
    }

    shared = (SharedData*) mmap(NULL, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        std::cout << "Jointenactor couldn't map to pointer!" << std::endl;
        exit(0);
    }
}

JointEnactorModule::~JointEnactorModule()
{
    // Close shared memory
    munmap(shared, sizeof(SharedData));
    close(shared_fd);
}

void JointEnactorModule::writeCommand()
{
    std::cout << "Joint Enactor~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Joints " << latestJointAngles_.DebugString() << std::endl; 
    std::cout << "Stiff " << latestStiffness_.DebugString() << std::endl; 
    std::cout << "Leds " << latestLeds_.DebugString() << std::endl; 
    std::cout << "done~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

    std::vector<SerializableBase*> objects = {
        new ProtoSer(&latestJointAngles_),
        new ProtoSer(&latestStiffness_),
        new ProtoSer(&latestLeds_)
    };
    std::cout << "Objects size: " << objects.size() << std::endl;

    commandIndex++;

    HighResTimer timer;
    // TODO grab semaphore
    int index = shared->commandSwitch ? 0 : 1;
    lastRead = shared->commandReadIndex;

    size_t usedSpace = 0;
    bool returned = serializeTo(objects, commandIndex, shared->command[index], COMMAND_SIZE, &usedSpace);
    if (!returned) {
        std::cout << "Serialization failed!!" << std::endl;
    }
    std::cout << "JointModule used: " << usedSpace << "bytes, to index: " << index << std::endl;
    shared->commandSwitch = index;
    // TODO release semaphore
    double time = timer.end();
    std::cout << "Enactor critical section took: " << time << std::endl;
    exit(0);

    if (commandIndex - lastRead > 10 && (lastRead < commandIndex)) {
        std::cout << "Commands aren't getting read! Did Boss die?" << std::endl;
        std::cout << "commandIndex: " << commandIndex << " lastRead: " << lastRead << std::endl;
        exit(0);
    }
}

void JointEnactorModule::run_()
{
    PROF_ENTER(P_JOINT_ENACTOR);

    // Update stiffnesses.
    stiffnessInput_.latch();
    latestStiffness_ = stiffnessInput_.message();

    // Update joint angles.
    jointsInput_.latch();
    latestJointAngles_ = jointsInput_.message();

    // Update leds
    ledsInput_.latch();
    latestLeds_ = ledsInput_.message();

    // Send them over to Boss
    writeCommand();

    PROF_EXIT(P_JOINT_ENACTOR);
}


} // namespace jointenactor
} // namespace man
