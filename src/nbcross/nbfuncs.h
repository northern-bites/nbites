//
//  nbfuncs.h
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//  Copyright (c) 2014 pkoch. All rights reserved.
//

#ifndef __nbcross__nbfuncs__
#define __nbcross__nbfuncs__

#include <stdio.h>
#include <iostream>
#include <vector>

typedef struct nbfunc_s {
    char * name;
    int (*func)(void) ;
    
    int num_args;
    char ** arg_names;
} nbfunc_t;

extern std::vector<nbfunc_t> FUNCS;

typedef struct nbopaquelog_s {
    char * desc;
    size_t data_len;
    uint8_t * data;
} nbopaquelog_t;

extern std::vector<nbopaquelog_t> args;
extern std::vector<nbopaquelog_t> rets;

void register_funcs();
void check_arguments(int func_index); //Checks that the arguments in <std::vector args> match
        //those listed in the func declaration FUNCS[func_index]

#endif /* defined(__nbcross__nbfuncs__) */
