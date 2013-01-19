#include "Threads.h"

namespace man {

/********   Cognition Thread  ********/

CognitionThread::CognitionThread() : Thread("cognition"),
                                     cognitionDiagram(),
                                     imageTranscriber(),
                                     logger()
{
    logger.topImageIn.wireTo(&imageTranscriber.topImageOut);
    cognitionDiagram.addModule(imageTranscriber);
    cognitionDiagram.addModule(logger);
}

void CognitionThread::run()
{
//    synchro::Thread::running = true;
//    synchro::Thread::trigger->on();

    while (Thread::running)
    {
        cognitionDiagram.run();
    }
}

/********   Comm Thread   ********/

CommThread::CommThread() : Thread("comm"),
                           commDiagram(),
                           comm()
{
    commDiagram.addModule(comm);
}

void CommThread::run()
{
//    synchro::Thread::running = true;
//    synchro::Thread::trigger->on();

    while (Thread::running)
    {
        commDiagram.run();
    }
}

}
