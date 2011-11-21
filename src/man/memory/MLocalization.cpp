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
        {//Octavian had afunny object decleration before brackets in MVisionSnsr
        }

        MLocalization::~MLocalization() {
        }

        void MLocalization::update() {
            //What is this?
            ADD_PROTO_TIMESTAMP;

