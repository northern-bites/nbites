#include "JointEnactorModule.h"
#include "Profiler.h"

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
        // TODO error
    }
    // if (ftruncate(shared_fd, sizeof(SharedData)) == -1) {
    //     std::cout << "Jointenactor couldn't truncate!" << std::endl;
    //     // TODO error
    // }

    shared = (SharedData*) mmap(NULL, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        std::cout << "Jointenactor couldn't map to pointer!" << std::endl;
        // TODO error
    }
}

JointEnactorModule::~JointEnactorModule()
{
    // Close shared memory
    munmap(shared, sizeof(SharedData));
    close(shared_fd);
    //sem_close(semaphore);
}

void JointEnactorModule::writeCommand()
{
    // JointCommand command;
    // command.jointsCommand = latestJointAngles_;
    // command.stiffnessCommand = latestStiffness_;
    // command.writeIndex = ++commandIndex;

    std::vector<SerializableBase*> objects = {
        new ProtoSer(&latestJointAngles_),
        new ProtoSer(&latestStiffness_),
        new ProtoSer(&latestLeds_)
    };
    // writeIndex = ++commandIndex
    commandIndex++;

    // TODO grab semaphore
    int index = shared->commandSwitch ? 0 : 1;
    lastRead = shared->commandReadIndex;

    size_t usedSpace;
    bool returned = serializeTo(objects, commandIndex, shared->command[index], COMMAND_SIZE, &usedSpace);
    shared->commandSwitch = index;
    // TODO release semaphore

    if (commandIndex - lastRead > 10 && (lastRead < commandIndex)) {
        std::cout << "Commands aren't getting read!!" << std::endl;
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

    ledsInput_.latch();
    latestLeds_ = ledsInput_.message();

    PROF_EXIT(P_JOINT_ENACTOR);
}


} // namespace jointenactor
} // namespace man
