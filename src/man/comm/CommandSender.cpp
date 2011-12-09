#include "CommandSender.h"

// Prints for debugging
//#define DEBUG_COMMAND

using std::string;
using std::cout;

CommandSender::CommandSender() : latestCmd(),
                                 changed(false)
{
}

void CommandSender::update(string newCmd)
{
    latestCmd = newCmd;
    changed = true;

#ifdef DEBUG_COMMAND
    cout << "New command received by sender: " << latestCmd << "\n";
#endif
}
