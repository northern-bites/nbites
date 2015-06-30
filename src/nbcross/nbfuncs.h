//
//  nbfuncs.h
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//

#ifndef __nbcross__nbfuncs__
#define __nbcross__nbfuncs__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Log.h"
#include "nbdebug.h"

#define NBCROSS_VERSION 6

class CrossFunc {
    
public:
    std::string name;
    int (*func)(void);
    std::vector<std::string> args;
    
    CrossFunc(std::string n,  int (*f)(void), std::vector<std::string> a) :
    name(n),
    func(f),
    args(a)
    { }
};

const std::string NBCROSS_WILDCARD_TYPE = "__WILDCARD__";

// Defined in pkoch_defs.cpp
int test_func();
int arg_test_func();
int CrossBright_func();

// Defined in misc_defs.cpp

// Defined in vision_defs.cpp
int Vision_func();
int CameraCalibration_func();
int Synthetics_func();
int Scratch_func();


/* add all functions to publish to this vector */
const std::vector<CrossFunc> FUNCS = {
    CrossFunc("test", test_func, {}),
    CrossFunc("arg_test", arg_test_func, {"YUVImage", "YUVImage"}),
    CrossFunc("CrossBright", CrossBright_func, {"YUVImage"}),
    CrossFunc("Vision", Vision_func, {"YUVImage"}),
    CrossFunc("CameraCalibration", CameraCalibration_func, {"YUVImage", "YUVImage", "YUVImage", 
                                                "YUVImage", "YUVImage", "YUVImage", "YUVImage"}),
    CrossFunc("Synthetics", Synthetics_func, {"SyntheticParams"}),
    CrossFunc("Scratch", Scratch_func, {})
};

extern std::vector<nblog::Log *> args;
extern std::vector<nblog::Log *> rets;

// Templated helper function for converting from big endian representation
// to little endian representation and vice versa.
template <class T>
void endswap(T *objp)
{
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}

#endif /* defined(__nbcross__nbfuncs__) */
