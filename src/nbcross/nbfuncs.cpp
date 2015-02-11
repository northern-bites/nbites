//
//  nbfuncs.cpp
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//  Copyright (c) 2014 pkoch. All rights reserved.
//

#include "nbfuncs.h"
#include <assert.h>
#include <vector>

std::vector<nbfunc_t> FUNCS;

std::vector<logio::log_t> args;
std::vector<logio::log_t> rets;

//Common arg types -- used to check arg types and for human readability.
const char sYUVImage[] = "YUVImage";
const char sParticleSwarm_pbuf[] = "ParticleSwarm";
const char sParticle_pbuf[] = "Particle";
const char sTest[] = "test";

const char stext[] = "text";//No current sources for this data type.

int test_func() {
    assert(args.size() == 2);
    for (int i = 0; i < args.size(); ++i) {
        printf("test_func(): %s\n", args[i].desc);
    }
    
    return 0;
}

int arg_test_func() {
    printf("arg_test_func()\n");
    assert(args.size() == 2);
    for (int i = 0; i < 2; ++i) {
        printf("\t%s\n", args[i].desc);
        rets.push_back(logio::copyLog(&args[i]));
    }
    
    return 0;
}

int CrossBright_func() {
    assert(args.size() == 1);
    printf("CrossBright_func()\n");
    //work on a copy of the arg so we can safely push to rets.
    logio::log_t log = logio::copyLog(&args[0]);
    for (int i = 0; i < log.dlen; i += 2) {
        *(log.data + i) = 240;
    }
    
    printf("[%s] modified.\n", log.desc);
    rets.push_back(log);
    
    return 0;
}

void register_funcs() {
    
    /*test func 1*/
    nbfunc_t test;
    test.name = (const char *) "simple test";
    test.args = {sTest, sTest};
    test.func = test_func;
    FUNCS.push_back(test);
    
    /*test func 2*/
    nbfunc_t arg_test;
    arg_test.name = (char *) "arg test";
    arg_test.args = {sYUVImage, sYUVImage};
    arg_test.func = arg_test_func;
    FUNCS.push_back(arg_test);
    
    //CrossBright
    nbfunc_t CrossBright;
    CrossBright.name = "CrossBright";
    CrossBright.args = {sYUVImage};
    CrossBright.func = CrossBright_func;
    FUNCS.push_back(CrossBright);
}


