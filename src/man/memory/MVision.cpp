
#include "MVision.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using std::list;
using std::vector;
using proto::PVision;

MVision::MVision(std::string name) :
        TemplatedProtobufMessage<PVision>(name) {
}

MVision::~MVision() {
}

void MVision::updateData() {

//
}

}
}
