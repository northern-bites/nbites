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

    typedef boost::shared_ptr<proto::PLoc> PLoc_ptr;

public:
    MLocalization(boost::shared_ptr<LocSystem> locSystem,
                  PLoc_ptr = PLoc_ptr(new proto::PLoc));
    virtual ~MLocalization();

    /**
     * Updates all the fields of the proto::PLoc
     * with values from the LocSystem pointer
     */
    void updateData();

private:
    boost::shared_ptr<LocSystem> locSystem;
    PLoc_ptr data;
};
}
}
