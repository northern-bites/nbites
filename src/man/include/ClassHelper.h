/**
 * ClassHelper.h
 *
 * Useful macros when declaring classes
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>

#define ADD_SHARED_PTR(class_name) \
public:\
	typedef boost::shared_ptr<class_name> ptr;\
	typedef boost::shared_ptr<const class_name> const_ptr;

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


#define ADD_NULL_INSTANCE(class_name) \
public:\
    static boost::shared_ptr<class_name> NullInstanceSharedPtr() {\
            static boost::shared_ptr<class_name> NullInstancePtr(new class_name());\
            return NullInstancePtr;\
    }\
    static class_name* NullInstance() {\
        return NullInstanceSharedPtr().get();\
    }\
    static bool isTheNullInstance(class_name* instance) {\
        return instance == NullInstance();\
    }
