/**
 * Walk commands take parameters in the following units:
 *   - Centimeter/second for Distance, Degrees/second for rotation.
 */
#pragma once

#include <boost/shared_ptr.hpp>

#include "MotionConstants.h"
#include "MotionCommand.h"

namespace man
{
namespace motion
{
class WalkInPlaceCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<WalkInPlaceCommand> ptr;

    WalkInPlaceCommand()
        : MotionCommand(MotionConstants::WALK_IN_PLACE)
        { setChainList(); }

    virtual ~WalkInPlaceCommand() {}
public:

protected:
	virtual void setChainList() {
    }

public:
    friend std::ostream& operator<< (std::ostream &o, const WalkInPlaceCommand &w)
        {
            return o << "WalkInPlaceCommand(";
        }

};
}
}
