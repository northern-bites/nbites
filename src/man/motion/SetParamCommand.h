#pragma once

/**
 * Destination commands take parameters in the following units:
 * X/Y mm relative to our current position to walk to, rads to turn to
 */
#include "MotionConstants.h"
#include "MotionCommand.h"

namespace man
{
namespace motion
{
class SetParamCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<SetParamCommand> ptr;

    SetParamCommand(float _param1, float _param2)
        : MotionCommand (MotionConstants::SET_PARAMS),
      param1(_param1),param2(_param2)
    {
      //setChainList();
      //call the method to change parameters here?
      cout << "Modifying Parameters..." << endl;
    }

    virtual ~SetParamCommand() {}

public:
    //SetParamCommand parameters
    const float param1;
    const float param2;
    /*
protected:
    virtual void setChainList() {
        chainList.assign(MotionConstants::WALK_CHAINS,
                         MotionConstants::WALK_CHAINS +
                         MotionConstants::WALK_NUM_CHAINS);
    }
    */
public:
    friend std::ostream& operator<< (std::ostream &o, const SetParamCommand &w)
        {
            return o << "SetParamCommand("
                     << w.param1 << "," << w.param2;
        }

};
} // namespace motion
} // namespace man
