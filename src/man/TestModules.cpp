#include "TestModules.h"
#include <iostream>

void FirstModule::run_()
{
    TestyTest* thing = new TestyTest();
    thing->set_something("hey");
    thing->set_blaaah(12);
    out.setMessage(Message<TestyTest>(thing));
}

void SecondModule::run_()
{
    in.latch();
    std::cout << in.message().DebugString() << std::cout;
}
