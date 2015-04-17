//
//  nbfuncs.cpp
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//

#include "nbfuncs.h"
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

std::vector<nbfunc_t> FUNCS;

std::vector<logshare::Log *> args;
std::vector<logshare::Log *> rets;

//Common arg types -- used to check arg types and for human readability.
const char sYUVImage[] = "YUVImage";
const char sParticleSwarm_pbuf[] = "ParticleSwarm";
const char sParticle_pbuf[] = "Particle";
const char sTest[] = "test";

const char stext[] = "text";//No current sources for this data type.

int test_func() {
    assert(args.size() == 2);
    for (int i = 0; i < args.size(); ++i) {
        printf("test_func(): %s\n", args[i]->description().c_str());
    }
    
    return 0;
}

int arg_test_func() {
    printf("arg_test_func()\n");
    assert(args.size() == 2);
    for (int i = 0; i < 2; ++i) {
        printf("\t%s\n", args[i]->description().c_str());
        rets.push_back(new logshare::Log(args[i]));
    }
    
    return 0;
}

int CrossBright_func() {
    assert(args.size() == 1);
    printf("CrossBright_func()\n");
    //work on a copy of the arg so we can safely push to rets.
    
    logshare::Log * copy = new logshare::Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);
    
    for (int i = 0; i < length; i += 2) {
        *(buf + i) = 240;
    }
    std::string buffer((const char *) buf, length);
    copy->setData(buffer);
    
    printf("[%s] modified.\n", copy->description().c_str());
    rets.push_back(copy);
    
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




#if __LINE__ > 100
#error "USE YOUR OWN FILES FOR FUNCTIONS!"
#endif