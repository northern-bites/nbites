
#pragma once

#include <boost/shared_ptr.hpp>

#define ADD_NULL_INSTANCE(class_name) \
    static class_name* NullInstance() {\
        static boost::shared_ptr<class_name>NullInstancePtr(new class_name());\
        return NullInstancePtr.get();\
    }
