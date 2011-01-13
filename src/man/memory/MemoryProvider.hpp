/*
 * MemoryProvider.hpp
 *
 * Virtual class that all objects that want to write to memory should implement
 *
 * @author Octavian Neamtu
 * @email <oneamtu@bowdoin.edu>
 */
#pragma once

#include "MObject.hpp"
#include <boost/shared_ptr.hpp>

template <class T>
class MemoryProvider {

protected:
    virtual void updateMemory(boost::shared_ptr<T> obj) = 0;
};
