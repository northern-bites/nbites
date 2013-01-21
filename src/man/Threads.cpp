#include "Threads.h"

namespace man {

CognitionThread::CognitionThread() : Thread("cognition"),
                                     cognitionDiagram(),
                                     imageTranscriber(),
                                     logger(),
				     visMod()
{
    logger.topImageIn.wireTo(&imageTranscriber.topImageOut);
    visMod.bottomImageIn.wireTo(&imageTranscriber.bottomImageOut);
    visMod.topImageIn.wireTo(&imageTranscriber.topImageOut);
    cognitionDiagram.addModule(imageTranscriber);
    cognitionDiagram.addModule(logger);
    cognitionDiagram.addModule(visMod);
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
