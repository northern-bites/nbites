/**
 *
 *@class MLocalization : This is the memory reprensentation of Loc
 *
 * @author EJ Googins
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Loc.pb.h"
#include "MObject.h"
#include "LocSystem.h"

namespace man{
    namespace memory {

        class MLocalization: public MObject {

        public:
            MLocalization(MObject_ID id, boost::shared_ptr<LocSystem> l,
                           boost::shared_ptr<proto::PLoc> loc_l_data);
            virtual ~MLocalization();

            /**
             * Updates all the fields of the proto::PLoc
             * with values from the LocSystem pointer
             */
            void update();

        private:
            boost::shared_ptr<LocSystem> locSystem;
            boost::shared_ptr<proto::PLoc> data;
        };
    }
}
