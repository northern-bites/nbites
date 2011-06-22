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

MImage::MImage(shared_ptr<Sensors> s) : RoboImage(), sensors(s) {
    //string* image_string = this->mutable_image();
    //image_string->assign(NAO_IMAGE_BYTE_SIZE * sizeof(char), 'a');
    //cout << " string capacity " << NAO_IMAGE_BYTE_SIZE << " "<<  image_string->capacity() << endl;
    //char* image_string_data = const_cast<char *>(image_string->data());
    //sensors->setNaoImagePointer(image_string_data);
    REGISTER_MOBJECT("Image");
}

MImage::~MImage() {
}

void MImage::update() {

    //ADD_PROTO_TIMESTAMP;
//    cout << "MImage_updata timestamp is " << process_micro_time() << endl;
    this->updateImage(sensors->getRawNaoImage());

//    string* image_string =  this->mutable_image();
//    image_string->assign((char *) (sensors->getNaoImage()), 640*480*sizeof(char)*2);
    //string* image_string = this->mutable_image();
    //TODO: const stripping is unsafe

//    memcpy(unsafe_data, sensors->getNaoImage(), sizeof(void*));
//    printf("%p %p %p %p \n", &c[0], data, unsafe_data, s->data());
    //cout << unsafe_data << " " << sensors->getNaoImage() << endl;

}
}
}
