#include "UnlogModule.h"
#include <errno.h>

namespace tool {
namespace unlog {

// Simple constructor, stores the file name
UnlogBase::UnlogBase(std::string path, std::string type) : Module(),
                                                           file(0),
                                                           fileName(path),
                                                           typeName(type)
{
}

// Closes the file as it destructs
UnlogBase::~UnlogBase()
{
    closeFile();
}

// Open the file for reading
void UnlogBase::openFile() throw (file_exception)
{
    file = fopen(fileName.c_str(), "rb");

    if (file == NULL) {
        throw file_exception(file_exception::CREATE_ERR, errno);
    }
}

// Close the file
void UnlogBase::closeFile()
{
    if (file) {
        fclose(file);
        file = 0;
    }
}

/*
 * The hardworking read method.
 *
 * @param buffer The buffer that will be filled from the file
 * @param size The number of bytes that should be read
 */
uint32_t UnlogBase::readCharBuffer(char* buffer, uint32_t size)
    const throw (file_read_exception)
{
    // Make sure we're taking from an open file
    if (!file) {
        throw file_read_exception(file_read_exception::NOT_OPEN);
    }

    // Store how many bytes were read so that we can return it
    uint32_t bytes = fread(buffer, sizeof(char), size, file);

    if (ferror(file)) {
        throw file_read_exception(file_read_exception::READ, ferror(file));
    }

    return bytes;
}

// This is stupid copy-pasting
uint32_t UnlogBase::readCharBuffer(unsigned char* buffer, uint32_t size)
    const throw (file_read_exception)
{
    // Make sure we're taking from an open file
    if (!file) {
        throw file_read_exception(file_read_exception::NOT_OPEN);
    }

    // Store how many bytes were read so that we can return it
    uint32_t bytes = fread(buffer, sizeof(unsigned char), size, file);

    if (ferror(file)) {
        throw file_read_exception(file_read_exception::READ, ferror(file));
    }

    return bytes;
}

template<>
messages::YUVImage UnlogModule<messages::YUVImage>::readNextMessage()
{
    // End of file
    if (feof(file))
    {
        std::cout << "End of log file " << fileName << std::endl;
        // This returning a blank image causes an assertion to fail
        // in other classes at end/beginning of log files.
        // FIXME
        return messages::YUVImage();
    }

    // Read in the next message's size
    // @see LogModule.h for why this works
    uint32_t currentSize = readValue<uint32_t>();

    // Keep track of the sizes we've read (to unwind)
    messageSizes.push_back(currentSize);

    // To hold the data read, and the number of bytes read
    uint32_t bytes;
    //
    messages::YUVImage* img = new messages::YUVImage(currentSize/480, 480);

    try {
        // Actual file reading call
        bytes = readCharBuffer(img->pixelAddress(0, 0), currentSize);
    } catch (std::exception& read_exception) {
        std::cout << read_exception.what() << std::endl;
        return messages::YUVImage();
    }

    if (bytes) {
        return *img;
    }

    // We read zero bytes at the end of a file w/o hitting feof
    std::cout << "End of log file " << fileName << std::endl;
    return *img;
}

//inverses the above message, with a few minor differences
template<>
messages::YUVImage UnlogModule<messages::YUVImage>::readPrevMessage() {
    if (ftell(file)==0) {
        std::cout << "Beginning of log file " << fileName << std::endl;
        return messages::YUVImage();
    }

    //we've been storing the message sizes to use right now
    uint32_t currentSize = messageSizes.back();
    messageSizes.pop_back();

    // To hold the data read, and the number of bytes read
    uint32_t bytes;
    messages::YUVImage* img = new messages::YUVImage(currentSize/480, 480);

    try {
        // Actual file reading call
        //set the file pointer BACK currentMessageSize bits
        fseek(file, -1*currentSize, SEEK_CUR);
        //then read forward
        bytes = readCharBuffer(img->pixelAddress(0, 0), currentSize);
        //then set it back again, so it appears that we read backwards
        //plus rewind four bites to get past the thing that says the size of the frame
        fseek(file, -1*(currentSize+sizeof(int)), SEEK_CUR);
    } catch (std::exception& read_exception) {
        std::cout << read_exception.what() << std::endl;
        return messages::YUVImage();
    }

    if (bytes) {
        return *img;
    }

    // We read zero bytes at the end of a file w/o hitting feof
    std::cout << "Beginning of log file " << fileName << std::endl;
    return *img;
}

template<>
UnlogModule<messages::YUVImage>::UnlogModule(std::string path) :
    UnlogBase(path, "messages.YUVImage"),
    output(base())
{
}

template<>
GenericProviderModule* UnlogModule<messages::YUVImage>::makeCorrespondingProvider()
{
    return new ImageProviderModule("messages.YUVImage");
}

}
}
