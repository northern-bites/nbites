#include "UnlogModule.h"
#include <errno.h>

namespace tool {
namespace unlog {

// Simple constructor, stores the file name
UnlogBase::UnlogBase(std::string path, std::string type) : Module(),
                                                           fileName(path),
                                                           typeName(type)
{}

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

    fileOpen = true;
}

// Close the file
void UnlogBase::closeFile()
{
    if (fileOpen) {
        fileOpen = false;
        fclose(file);
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
    if (!fileOpen) {
        throw file_read_exception(file_read_exception::NOT_OPEN);
    }

    // Store how many bytes were read so that we can return it
    uint32_t bytes = fread(buffer, sizeof(char), size, file);

    if (ferror(file)) {
        throw file_read_exception(file_read_exception::READ, ferror(file));
    }

    return bytes;
}

}
}
