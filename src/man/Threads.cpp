#include "Threads.h"
#include "RobotConfig.h"

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
    while (Thread::running)
    {
        cognitionDiagram.run();
    }
}

/********   Comm Thread   ********/

CommThread::CommThread() : Thread("comm"),
                           commDiagram(),
                           comm(MY_TEAM_NUMBER, MY_PLAYER_NUMBER)
{
    commDiagram.addModule(comm);
}

void CommThread::run()
{
    while (Thread::running)
    {
        commDiagram.run();
    }
}

}
