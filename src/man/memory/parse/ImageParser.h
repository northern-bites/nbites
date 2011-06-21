/**
 * @class ImageParser
 *
 * Parses an image stream log
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <unistd.h>
#include <iostream>
#include <string>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <stdint.h>

#include "Parser.h"
#include "memory/RoboImage.h"

namespace man {
namespace memory {
namespace parse {

namespace proto_io = google::protobuf::io;

class ImageParser : Parser <RoboImage>{

public:
    ImageParser(boost::shared_ptr<RoboImage> image,
               const char* _file_name);
//    ImageParser(boost::shared_ptr<const RoboImage> image,
//               int _file_descriptor);

    ~ImageParser();

    void initStreams();

    const LogHeader getHeader();
    boost::shared_ptr<const RoboImage> getNext();
    boost::shared_ptr<const RoboImage> getPrev();

private:
    void getNextBuffer();
    void readHeader();
    /*
     * Writes a value of type T to the current buffer.
     * It writes it at the specified offset and then
     * increments the offset with the size of the value written.
     */
    template <class T>
    T readValue(uint32_t* offset) {
        const uint8_t* byte_ptr_to_read_from =
                        reinterpret_cast<const uint8_t*> (*current_buffer)
                        + *offset;
        T value = *(reinterpret_cast<const T*> (byte_ptr_to_read_from));
        *offset += sizeof(T);
        return value;
    }

private:
    const void** current_buffer;
    int current_buffer_size;
    int file_descriptor;
    bool finished;

    proto_io::FileInputStream* raw_input;

};

}
}
}
