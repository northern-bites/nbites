/*
 * Simple class that can be used to send a string to Python from the tool.
 * Wrapped into Python via PySender. Initialized in and works closely
 * with TOOLConnect.
 *
 * @author Lizzie Mamantov
 */

#ifndef COMMANDSENDER_H
#define COMMANDSENDER_H

#include <string>
#include <iostream>

class CommandSender
{
private:
    std::string latestCmd;
    bool changed;

public:
    CommandSender();
    ~CommandSender() {};

    void update(std::string newCmd);

    // Getters, setters
    bool hasNewCommand() { return changed; }
    void setChanged(bool yn) { changed = yn; }
    std::string getCommand() { return latestCmd; }

};

#endif
