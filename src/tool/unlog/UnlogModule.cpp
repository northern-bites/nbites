#include "UnlogModule.h"
#include <errno.h>

namespace tool {
namespace unlog {

using namespace man::log;

UnlogBase::UnlogBase(std::string path) : Module(),
                                         fileName(path)
{}

UnlogBase::~UnlogBase()
{
    closeFile();
}

void UnlogBase::openFile() throw (file_exception)
{
    file = fopen(fileName.c_str(), "rb");

    if (file == NULL) {
        throw file_exception(file_exception::CREATE_ERR, errno);
    }

    fileOpen = true;
}

void UnlogBase::closeFile()
{
    if (fileOpen) {
        fileOpen = false;
        fclose(file);
    }
}

uint32_t UnlogBase::readCharBuffer(char* buffer, uint32_t size)
    const throw (file_read_exception)
{
    if (!fileOpen) {
        throw file_read_exception(file_read_exception::NOT_OPEN);
    }

    uint32_t bytes = fread(buffer, sizeof(char), size, file);

    if (ferror(file)) {
        throw file_read_exception(file_read_exception::READ, ferror(file));
    }

    return bytes;
}

}
}
