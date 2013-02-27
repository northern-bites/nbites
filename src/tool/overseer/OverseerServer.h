/**
 * @class OverseerServer
 *
 * Simple server class that listens for an inbound connection,
 * and then starts a threaded logger that streams current ground truth data to the client
 *
 * @author Octavian Neamtu
 *
 */


#pragma once

#include <vector>

#include "Structs.h"
#include "io/MessageInterface.h"
#include "synchro/synchro.h"

//skip including generated man configuration files 
#define NO_MANCONFIG

#include "OverseerDef.h"
#include "man/memory/log/MessageLogger.h"

namespace nbites {
namespace overseer {

class OverseerServer : public Thread {

    typedef std::vector<man::memory::log::MessageLogger::ptr> Loggers;

public:

    OverseerServer(point<float>* ballPosition,
                   std::vector<point<float> >* robotPositions);
    virtual ~OverseerServer();

    virtual void run();

    virtual void postData();

private:
    common::io::MessageInterface::ptr groundTruthMessage;
    Loggers loggers;

};

}
}
