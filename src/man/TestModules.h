// This is just a test!!! Don't leave this is in the code base!!!

#pragma once
#include "Test.pb.h"
#include "RoboGrams.h"

class FirstModule : public Module
{
public:
    FirstModule() : Module(),
                    out(base()) {}
    OutPortal<TestyTest> out;

protected:
    virtual void run_();
};

class SecondModule : public Module
{
public:
    SecondModule() : Module() {}
    InPortal<TestyTest> in;

protected:
    virtual void run_();
};
