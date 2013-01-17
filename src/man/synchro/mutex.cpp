
#include "mutex.h"

#include <errno.h>
#include <iostream>

using namespace std;

mutex::mutex(std::string name) : name(name){
#ifdef DEBUG_MUTEX
    static pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    pthread_mutex_init(&my_mutex, &attr);
#else
    pthread_mutex_init(&my_mutex, NULL);
#endif
}
mutex::~mutex() {
    pthread_mutex_destroy(&my_mutex);
}
void mutex::lock() const {
#ifdef DEBUG_MUTEX
    static int try_counter = 0;
    static timespec interval, remainder;
    static bool announced = false;
    while (pthread_mutex_trylock(&my_mutex) == EBUSY) {
        try_counter++;
        if (try_counter > 20 && !announced) {
            cout << "Possibly slow mutex in: " << name << endl;
            announced = true;
        }
        interval.tv_nsec = 50;
        interval.tv_sec = 0;
        nanosleep(&interval, &remainder);
    }
#else
    pthread_mutex_lock(&my_mutex);
#endif
}
void mutex::unlock() const {
    pthread_mutex_unlock(&my_mutex);
}

//multi_mutex

multi_mutex::multi_mutex(mutex* mutex_array[], std::string name) :
                mutices(mutex_array,
                        mutex_array + sizeof(mutex_array) / sizeof(mutex*)),
                name(name){}
void multi_mutex::lock() {
    for (mutex_vector::iterator i = mutices.begin();
            i != mutices.end(); i++) {
        (*i)->lock();
    }
}
void multi_mutex::unlock() {
    for (mutex_vector::reverse_iterator i = mutices.rbegin();
            i != mutices.rend(); i++) {
        (*i)->unlock();
    }
}
