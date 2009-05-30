#ifndef BaseFreezeCommand_h
#define BaseFreezeCommand_h

#include "MotionCommand.h"

class BaseFreezeCommand : public MotionCommand {
public:
    BaseFreezeCommand(float stiffness);
    virtual ~BaseFreezeCommand();

    float getStiffness();
private:
    void setChainList();
    float targetStiffness;
};

#endif
