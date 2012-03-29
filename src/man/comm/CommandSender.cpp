#include "CommandSender.h"

// Prints for debugging
//#define DEBUG_COMMAND

using std::string;
using std::cout;

CommandSender::CommandSender() : latestCmd(),
                                 changed(false)
{
}

// Gets called when a new command is found.
void CommandSender::update(string newCmd)
{
    latestCmd = newCmd;
    // Python will set this back to false after cmd is recieved.
    changed = true;

#ifdef DEBUG_COMMAND
    cout << "New command received by sender: " << latestCmd << "\n";
#endif
}
