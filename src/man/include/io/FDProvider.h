/**
 * @class FDProvider
 *
 * An abstract class that has the only role of providing a file descriptor
 * (somehow).
 *
 * On unix a file descriptor could represent anything from an actual file to
 * a socket or some shared memory stuff.
 *
 * So each child class of this class should implement some way of getting a
 * file descriptor.
 */


#pragma once


class FDProvider {

public:
    FDProvider() {};
    int getFileDescriptor() const { return file_descriptor; }

    virtual void openFileDescriptor() = 0;

protected:
    int file_descriptor;

};
