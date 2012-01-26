/*
 * MImage.cpp
 *
 *      Author: oneamtu
 */

#include <string>
#include <typeinfo>

#include "Common.h" //for micro_time
#include "MemoryMacros.h"
#include "MImage.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace std;
using proto::PImage;

MImage::MImage(MObject_ID id, shared_ptr<Sensors> s,
               shared_ptr<PImage> image_data) :
        MObject(id, image_data),
        sensors(s),
        data(image_data),
        thresholded_data(new PImage()){

    //Note (Octavian): This is a pretty dumb way to get the image data
    // (ideally you would want to just copy the image - that saves any
    // potential multi-threading safety issues and is how we update
    // ALL of the other memory objects)

    // Alas, copying the image means a 3 ms delay each frame, or roughly 10%
    // of regular running time; if we have too much free processing time on our
    // hands it would be prettier if we could avoid this

    string* image_string = this->data->mutable_image();
    // allocate the memory necessary for the image;
    image_string->assign(NAO_IMAGE_BYTE_SIZE * sizeof(char), 'a');
    char* image_string_data = const_cast<char *>(image_string->data());
    if (sensors.get()) {
        sensors->setNaoImagePointer(image_string_data);
    }
}

MImage::~MImage() {
}

void MImage::updateData() {

    ADD_PROTO_TIMESTAMP;
//    cout << "MImage_updata timestamp is " << process_micro_time() << endl;
    //Note: we don't need to update the image since it's set to already copy
    //into our image_string
    this->data->set_width(sensors->getRoboImage()->getWidth());
    this->data->set_height(sensors->getRoboImage()->getHeight());

    //debugging purposes
    #ifdef OFFLINE
    this->thresholded_data->mutable_image()->assign(
            reinterpret_cast<const char *>(sensors->getColorImage()),
            AVERAGED_IMAGE_SIZE);
    this->thresholded_data->set_width(AVERAGED_IMAGE_WIDTH);
    this->thresholded_data->set_height(AVERAGED_IMAGE_HEIGHT);
    #endif
}

}
}
