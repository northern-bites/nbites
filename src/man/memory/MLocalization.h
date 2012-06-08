/**
 *
 * @class MLocalization : This is the memory reprensentation of Loc
 *
 * @author EJ Googins
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Loc.pb.h"
#include "MemoryCommon.h"
#include "LocSystem.h"

namespace man {
namespace memory {

class MLocalization: public TemplatedProtobufMessage<proto::PLoc> {

public:
    MLocalization(std::string name, boost::shared_ptr<LocSystem> locSystem);
    virtual ~MLocalization();

    /**
     * Updates all the fields of the proto::PLoc
     * with values from the LocSystem pointer
     */
    void updateData();

private:
    boost::shared_ptr<LocSystem> locSystem;

};
}
}
