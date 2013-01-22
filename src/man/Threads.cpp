#include "Threads.h"

namespace man {

CognitionThread::CognitionThread() : Thread("cognition"),
                                     cognitionDiagram(),
                                     imageTranscriber(),
                                     logger()
{
    logger.topImageIn.wireTo(&imageTranscriber.topImageOut);
    logger.bottomImageIn.wireTo(&imageTranscriber.bottomImageOut);
    cognitionDiagram.addModule(imageTranscriber);
    cognitionDiagram.addModule(logger);
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
