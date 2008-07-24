#ifndef _HeadScanCommand_h_DEFINED
#define _HeadScanCommand_h_DEFINED

#include <vector>

#include "Kinematics.h"
#include "HeadJointCommand.h"

class HeadScanCommand {
 public:
  HeadScanCommand(const vector <const HeadJointCommand*> *_commands,
		  const bool _forever=false)
    : commands(_commands), forever(_forever) { }

  HeadScanCommand(const HeadScanCommand &other)
    : forever(other.forever) {
    vector <const HeadJointCommand*> *_commands =
        new vector<const HeadJointCommand*>();

    for (vector<const HeadJointCommand*>::const_iterator
         i=other.commands->begin(); i!=other.commands->end(); i++)
      _commands->push_back(new HeadJointCommand(**i));

    commands = _commands;
  }
  ~HeadScanCommand() {
    
    for (vector <const HeadJointCommand*>::const_iterator i = commands->begin();
	 i != commands->end(); ++i)
      delete *i;

    delete commands;
  }

  const vector <const HeadJointCommand*>* getCommands() const {
    return commands;
  }

  const bool isForever() const { return forever; }

 private:
  const vector <const HeadJointCommand*> *commands;
  const bool forever;
};

#endif
