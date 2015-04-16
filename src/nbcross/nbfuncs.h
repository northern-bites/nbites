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

#include "Log.h"
#include "nbdebug.h"

#define NBCROSS_VERSION 6

typedef struct {
    const char * name;
    int (*func)(void) ;
    
    std::vector<const char * > args;
} nbfunc_t;

extern std::vector<nbfunc_t> FUNCS;

extern std::vector<log::Log *> args;
extern std::vector<log::Log *> rets;

void register_funcs();
void check_arguments(int func_index); //Checks that the arguments in <std::vector args> match
        //those listed in the func declaration FUNCS[func_index]

#endif /* defined(__nbcross__nbfuncs__) */
