#include "Threads.h"

namespace man {

CognitionThread::CognitionThread() : Thread("cognition"),
                                     cognitionDiagram(),
                                     imageTranscriber()
{
    cognitionDiagram.addModule(imageTranscriber);
}

void CognitionThread::run()
{
    synchro::Thread::running = true;
    synchro::Thread::trigger->on();

    while (Thread::running)
    {
        cognitionDiagram.run();
    }
}

}
