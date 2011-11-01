/**
 * This file defines ADD_NULL_INSTANCE, a macro that will add a static function
 * to a class that returns a singleton instance of that class called the NullInstance
 *
 * The NullInstance is constructed with the default constructor of the class
 * The NullInstance is usually expected to do nothing, and is usually used
 * in combination with an interface
 *
 * It's a very neat alternative to having class pointers be initialized to NULL,
 * since you don't have to always check if the pointer is NULL, and might also
 * keep you away from some nasty segfaults
 *
 * @author Octavian Neamtu
 */


#pragma once

#include <boost/shared_ptr.hpp>

#define ADD_NULL_INSTANCE(class_name) \
    static class_name* NullInstance() {\
        static boost::shared_ptr<class_name>NullInstancePtr(new class_name());\
        return NullInstancePtr.get();\
    }\
    static bool isTheNullInstance(class_name* instance) {\
        return instance == NullInstance();\
    }
