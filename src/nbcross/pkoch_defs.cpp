#include "nbfuncs.h"
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

using nblog::Log;
using nblog::SExpr;

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
        rets.push_back(new Log(args[i]));
    }
    
    return 0;
}

int CrossBright_func() {
    assert(args.size() == 1);
    printf("CrossBright_func()\n");
    //work on a copy of the arg so we can safely push to rets.
    
    Log * copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);
    
    for (int i = 0; i < length; i += 2) {
        *(buf + i) = 240;
    }
    std::string buffer((const char *) buf, length);
    copy->setData(buffer);
    
    rets.push_back(copy);
    
    return 0;
}
