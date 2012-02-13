/**
 *
 * @class MLocalization : This is the memory reprensentation of Loc
 *
 * @author EJ Googins
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Loc.pb.h"
#include "MObject.h"
#include "LocSystem.h"

namespace man {
namespace memory {

class MLocalization: public MObject {

public:
    static const MObject_ID id = MLOCALIZATION_ID;

public:
    MLocalization(boost::shared_ptr<LocSystem> locSystem);
    virtual ~MLocalization();

    /**
     * Updates all the fields of the proto::PLoc
     * with values from the LocSystem pointer
     */
    void updateData();

private:
    boost::shared_ptr<LocSystem> locSystem;
    boost::shared_ptr<proto::PLoc> data;
};
}
}
