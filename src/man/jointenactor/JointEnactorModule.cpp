#include "JointEnactorModule.h"
#include "Profiler.h"
#include "HighResTimer.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "DebugConfig.h"

#define BOSS_DEAD_THRESHOLD 50

namespace man {
namespace jointenactor{

JointEnactorModule::JointEnactorModule()
{
    shared_fd = shm_open(NBITES_MEM, O_RDWR, 0600);
    if (shared_fd < 0) {
        std::cout << "Jointenactor couldn't open shared fd!" << std::endl;
        exit(0);
    }

    shared = (volatile SharedData*) mmap(NULL, sizeof(SharedData),
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
    munmap((void *)shared, sizeof(SharedData));
    close(shared_fd);
}

bool syncCmndWrite(volatile SharedData * sd, uint8_t * stage)
{
    pthread_mutex_t * cmutex = (pthread_mutex_t *) &sd->cmnd_mutex;
    pthread_mutex_lock(cmutex);

    memcpy((void *)sd->command, stage, COMMAND_SIZE);
    ++(sd->latestCommandWritten);

    pthread_mutex_unlock(cmutex);
    return true; // Should we ever return anything but true?
}

void JointEnactorModule::writeCommand()
{
    std::vector<SerializableBase*> objects = {
        new ProtoSer(&latestJointAngles_),
        new ProtoSer(&latestStiffness_),
        new ProtoSer(&latestLeds_)
    };
    
    if ( !serializeTo(objects, shared->latestCommandWritten + 1, cmndStaging, COMMAND_SIZE, NULL) ) {
        std::cout << "Serialization failed!!" << std::endl;
        return;
    }
    
    if ( !syncCmndWrite(shared, cmndStaging) ) {
        printf("Could not write out command!\n");
        return;
    }

    int64_t lw = shared->latestCommandWritten;
    int64_t lr = shared->latestCommandRead;

    if (lw - lr > 1) {
        std::cout << "BOSS missed a frame" << std::endl;
    }

    if ( (lw - lr) > BOSS_DEAD_THRESHOLD
        && (lr != 0)    //Boss might be slow on man startup, that's ok
        ) {
        std::cout << "Commands aren't getting read! Did Boss die?" << std::endl;
        std::cout << "commandIndex: " << lw << " lastRead: " << lr << std::endl;
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
