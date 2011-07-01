/**
 * @class mutex
 *
 * Simple C++ wrapper for pthread_mutex
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <pthread.h>
#include <string>
#include <vector>

class mutex {
public:
    mutex(std::string name);
    virtual ~mutex();

    void lock() const;
    void unlock() const;
private:
    mutable pthread_mutex_t my_mutex;
    std::string name;
};

class multi_mutex {
protected:
    typedef std::vector<mutex*> mutex_vector;
public:
    multi_mutex() {}
    multi_mutex(mutex* mutex_array[], std::string name);
    virtual ~multi_mutex(){}

    void lock();
    void unlock();
private:
    mutable mutex_vector mutices;
    std::string name;
};
