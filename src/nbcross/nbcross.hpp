//
//  nbcross.hpp
//  tool8-separate
//
//  Created by Philip Koch on 4/16/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#ifndef nbcross_hpp
#define nbcross_hpp

#include <stdio.h>

#include "Log.hpp"
#include "LogRPC.hpp"

#include "nblogio.h"
#include "utilities.hpp"


//Redefine NBL_PRINT_ to take NBCROSS_PRINTOUTS_ON into account
//will affect all derivative macros (NBL_WARN, NBL_ERROR, etc)
#undef NBL_PRINT_
#define NBL_PRINT_(LEVEL, format, ...)              \
    if (NBCROSS_PRINTOUTS_ON) { nbl_print(                                 \
    NBL_LOGGING_LEVEL,            \
    LEVEL,                    \
    __FILE__,                                       \
    __LINE__,                                       \
    format, ## __VA_ARGS__ );                       \
};

namespace nblog {
    namespace nbcross {
        /*
         Helper functions
         */

        SExpr readSexprFromFile(const std::string& filePath);
        json::Value readJsonFromFile(const std::string& filePath);

        // Templated helper function for converting from big endian representation
        // to little endian representation and vice versa.
        template <class T>
        void endswap(T *objp)
        {
            unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
            std::reverse(memp, memp + sizeof(T));
        }

        /* globals set in nbcross.cpp from commandline arguments */
        extern bool NBCROSS_PRINTOUTS_ON;
        extern std::string NBCROSS_INSTANCE_NAME;

        /*
         All nbcross functions must be delcared here, using the 
         NBCROSS_FUNCTION_DECL(name, va, ...)
         macro, where
            * name * refers to the 
         */
#define NBCROSS_FUNCTION_SET                                              \
    NBCROSS_FUNCTION_DECL(Test, true)                                     \
    NBCROSS_FUNCTION_DECL(Test2, false, CONSTANTS.LogClass_Null() )       \
    NBCROSS_FUNCTION_DECL(Vision, false, CONSTANTS.LogClass_Tripoint() )  \
    NBCROSS_FUNCTION_DECL(Calibrate, true, CONSTANTS.LogClass_Tripoint() )\
    
#define NBCROSS_FUNCTION_DECL(name, ava, ...)   \
        RPC_FUNCTION_DECLARE_COMPLETE(name, ava, ## __VA_ARGS__)

        NBCROSS_FUNCTION_SET

    }
}

#endif /* nbcross_hpp */
