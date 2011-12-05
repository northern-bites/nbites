/*
 *MLocalization.cpp
 *
 *@author EJ Googins
 */

#include "Common.h" //for micro_time
#include "MemoryMacros.h"
#include "MLocalization.h"

#include <vector>

namespace man {
    namespace memory {

        using boost::shared_ptr;
        using namespace proto;
        using namespace std;
        MLocalization::MLocalization(MObject_ID id, shared_ptr<LocSystem> l,
                                     shared_ptr<proto::PLoc> loc_l_data)
            : MObject(id, loc_l_data), locSystem(l), data(loc_l_data){
        }

        //Destructor
        MLocalization::~MLocalization() {
        }

        void MLocalization::update() {
             ADD_PROTO_TIMESTAMP;

             this->data->set_x_est(locSystem->getXEst());
             this->data->set_y_est(locSystem->getYEst());
             this->data->set_h_est(locSystem->getHEst());
             this->data->set_x_uncert(locSystem->getXUncert());
             this->data->set_y_uncert(locSystem->getYUncert());
             this->data->set_h_uncert(locSystem->getHUncert());
        }
    }
}
