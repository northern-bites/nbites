#include "JointEnactorModule.h"
#include "Profiler.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "DebugConfig.h"

namespace man {
namespace jointenactor{

JointEnactorModule::JointEnactorModule()
{
    shared_fd = shm_open(NBITES_MEM, O_RDWR, 0600);
    if (shared_fd < 0) {
        // TODO error
    }
    if (ftruncate(shared_fd, sizeof(SharedData)) == -1) {
        // TODO error
    }

    SharedData* sharedMem = (SharedData*) mmap(NULL, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, shared_fd, 0);

    if (shared == MAP_FAILED) {
        // TODO error
    }

    shared = sharedMem;

    semaphore = sem_open(NBITES_SEM, O_RDWR, 0600, 0);

    if (semaphore == SEM_FAILED) {
        // TODO error
    }
}

JointEnactorModule::~JointEnactorModule()
{
    // Close shared memory
    munmap(shared, sizeof(SharedData));
    close(shared_fd);
    sem_close(semaphore);
}

void JointEnactorModule::writeCommand()
{
    JointCommand command;
    command.jointsCommand = latestJointAngles_;
    command.stiffnessCommand = latestStiffness_;
    // TODO grab semaphore
    int index;
    shared->commands[index] = command;
    shared->leds[index] = latestLeds_;
    shared->commandsLatest = index;
    // TODO release semaphore
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
