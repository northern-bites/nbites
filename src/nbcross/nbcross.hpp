//
//  nbcross.hpp
//  tool8-separate
//
//  Created by Philip Koch on 4/16/16.
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
    if (nbl::nbcross::NBCROSS_PRINTOUTS_ON) { _NBUQ(nbl_print)(                                 \
    NBL_LOGGING_LEVEL,            \
    LEVEL,                    \
    __FILE__,                                       \
    __LINE__,                                       \
    format, ## __VA_ARGS__ );                       \
};

RPC_MAKE_FUNCTION_GROUP_VISIBLE(NBCross)

#define NBCROSS_FUNCTION(name, va, ...) \
    RPC_FUNCTION_DECLARE_COMPLETE(name, va, ## __VA_ARGS__) \
    RPC_FUNCTION_STATIC_ADD(name, RPC_FUNCTION_INSTANTIATE(name), NBCross)\
    RPC_FUNCTION_DEFINE(name)

//NOTE: after using the NBCROSS_FUNCTION() macro, you must
//specify the standard arguments:
//  (const std::vector<nbl::logptr> &arguments)
//(add that line to the above macro if we think it's dead obvious what defineCall() takes as args...)

// Templated helper function for converting from big endian representation
// to little endian representation and vice versa.
template <class T>
void endswap(T *objp)
{
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}

namespace nbl {

    // Helper functions
    SExpr readSexprFromFile(const std::string& filePath);
    json::Value readJsonFromFile(const std::string& filePath);

    namespace nbcross {

        /* globals set in nbcross.cpp from commandline arguments */
        extern bool NBCROSS_PRINTOUTS_ON;
        extern std::string NBCROSS_INSTANCE_NAME;

    }
}

#endif /* nbcross_hpp */
