#ifndef BaseFreezeCommand_h
#define BaseFreezeCommand_h

#include "MotionCommand.h"

namespace man
{
namespace motion
{
class BaseFreezeCommand : public MotionCommand 
{
public:
    BaseFreezeCommand(float stiffness);
    virtual ~BaseFreezeCommand();

    float getStiffness();
private:
    void setChainList();
    float targetStiffness;
};
} // namespace motion
} // namespace man

#endif
