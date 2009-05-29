#ifndef FreezeCommand_h
#define FreezeCommand_h

#include "MotionCommand.h"

class FreezeCommand : public MotionCommand {
public:
    FreezeCommand(float stiffness);
    virtual ~FreezeCommand();

    float getStiffness();
private:
    void setChainList();
    float targetStiffness;
};

#endif
