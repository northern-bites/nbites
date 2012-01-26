#include <fcntl.h>

namespace common {
namespace io {

enum OpenFlags {
    //O_APPEND is crucial if you want to use aio_write on the file descriptor
    //you open
    NEW = O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,
    EXISTING = O_RDONLY
};

enum OpenPermissions {
    PERMISSIONS_ALL = S_IRWXU | S_IRWXG | S_IRWXO
};
}
}
