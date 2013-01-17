#include "Threads.h"

namespace man {

TestThread::TestThread() : Thread("test"),
                           testDiagram(),
                           firstMod(),
                           secondMod()
{
    secondMod.in.wireTo(&firstMod.out);
    testDiagram.addModule(firstMod);
    testDiagram.addModule(secondMod);
}

void TestThread::run()
{
    testDiagram.run();
}

}
