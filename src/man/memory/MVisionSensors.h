///**
// * @class MVisionSensors : this is the memory object representation of vision Sensors
// *
// * @author Octavian Neamtu
// */
//
//#pragma once
//
//#include "protos/Sensors.pb.h"
//#include "MemoryCommon.h"
//#include "Sensors.h"
//#include "ClassHelper.h"
//
//namespace man {
//namespace memory {
//
//class MVisionSensors: public TemplatedProtobufMessage<proto::PVisionSensors> {
//
//ADD_SHARED_PTR(MVisionSensors);
//
//public:
//    MVisionSensors(std::string name, boost::shared_ptr<Sensors> sensors);
//    virtual ~MVisionSensors();
//
//    /**
//     * Updates all the fields of the underlying proto::PSensors with values
//     * from the Sensors object
//     */
//    void updateData();
//    void copyTo(boost::shared_ptr<Sensors> sensors) const;
//
//private:
//    boost::shared_ptr<Sensors> sensors;
//};
//}
//}
