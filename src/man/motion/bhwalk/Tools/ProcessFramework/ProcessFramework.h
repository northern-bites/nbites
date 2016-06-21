/**
* @file Tools/ProcessFramework/ProcessFramework.h
*
* This file declares classes corresponding to the process framework.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once

#include <list>
#include "PlatformProcess.h"
#include "Receiver.h"
#include "Sender.h"
#include "Tools/Streams/AutoStreamable.h"
#ifdef TARGET_SIM
#include "Controller/RoboCupCtrl.h"
#endif

/**
* The class is a helper that allows to instantiate a class as an Win32 process.
* ProcessBase contains the parts that need not to be implemented as a template.
* It will only be used by the macro MAKE_PROCESS and should never be used directly.
*/
class ProcessBase : public Thread<ProcessBase>
{
protected:
  /**
  * The main function of this Windows thread.
  */
  virtual void main() = 0;

public:
  /**
  * Virtual destructor.
  */
  virtual ~ProcessBase() {}

  /**
  * The function starts the process by starting the Windows thread.
  */
  void start() {Thread<ProcessBase>::start(this, &ProcessBase::main);}

  /**
  * The functions searches for a sender with a given name.
  * @param name The name of the sender.
  * @return If the sender was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */
  virtual SenderList* lookupSender(const std::string& name) = 0;

  /**
  * The functions searches for a receiver with a given name.
  * @param name The name of the receiver.
  * @return If the receiver was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */
  virtual ReceiverList* lookupReceiver(const std::string& name) = 0;

  /**
  * The function returns the name of the process.
  * @return The name of the process that normally is its class name.
  */
  virtual const std::string& getName() const = 0;

  /**
  * The function returns a pointer to the process if it has the given name.
  * @param processName The name of the process that is searched for.
  * @return If the process has the required name, a pointer to it is
  *         returned. Otherwise, the function returns 0.
  */
  virtual PlatformProcess* getProcess(const std::string& processName) = 0;
};

/**
* The class is a helper that allows to instantiate a class as an Win32 process.
* ProcessCreator contains the parts that need to be implemented as a template.
* It will only be used by the macro MAKE_PROCESS and should never be used directly.
*/
template<class T> class ProcessFrame : public ProcessBase
{
private:
  T process; /**< The process. */
  std::string name; /**< The name of the process. */

protected:
  /**
  * The main function of this Windows thread.
  */
  virtual void main()
  {
#ifdef TARGET_SIM
    NAME_THREAD((RoboCupCtrl::controller->getRobotName() + "." + name).c_str());
#endif

    // Call process.nextFrame if no blocking receivers are waiting
    setPriority(process.getPriority());
    process.processBase = this;
    Thread<ProcessBase>::yield(); // always leave processing time to other threads
    while(isRunning())
    {
      if(process.getFirstReceiver())
        process.getFirstReceiver()->checkAllForPackages();
      bool wait = process.processMain();
      if(process.getFirstSender())
        process.getFirstSender()->sendAllUnsentPackages();
      if(wait)
        process.wait();
    }
    process.terminate();
  }

public:
  /**
  * Constructor.
  * @param name The name of the process.
  */
  ProcessFrame(const std::string& name) : name(name) {}

  /**
  * The functions searches for a sender with a given name.
  * @param senderName The name of the sender.
  * @return If the sender was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */
  virtual SenderList* lookupSender(const std::string& senderName)
  {return process.getFirstSender() ? process.getFirstSender()->lookup(name, senderName) : 0;}

  /**
  * The functions searches for a receiver with a given name.
  * @param receiverName The name of the receiver.
  * @return If the receiver was found, a pointer to it is returned.
  *         Otherwise, the function returns 0.
  */
  virtual ReceiverList* lookupReceiver(const std::string& receiverName)
  {return process.getFirstReceiver() ? process.getFirstReceiver()->lookup(name, receiverName) : 0;}

  /**
  * The function returns the name of the process.
  * @return the name of the process.
  */
  virtual const std::string& getName() const {return name;}

  /**
  * The function returns a pointer to the process if it has the given name.
  * @param processName The name of the process that is searched for.
  * @return If the process has the required name, a pointer to it is
  *         returned. Otherwise, the function returns 0.
  */
  virtual PlatformProcess* getProcess(const std::string& processName)
  {
    if(name == processName)
      return &process;
    else
      return 0;
  }

  /**
   * The function announces that the thread shall terminate.
   * It will not try to kill the thread.
   */
  void announceStop()
  {
    Thread<ProcessBase>::announceStop();
    process.trigger();
  }
};

class ProcessList;

/**
* The class is a base class for process creators.
*/
class ProcessCreatorBase
{
private:
  static ProcessCreatorBase* first; /**< The head of the list of all process creators. */
  ProcessCreatorBase* next; /**< The next process creator in the list. */

protected:
  /**
  * The function creates a process.
  * @return A pointer to the new process.
  */
  virtual ProcessBase* create() const = 0;

public:
  /**
  * Constructor.
  */
  ProcessCreatorBase() : next(first) {first = this;}

  /**
  * Virtual destructor.
  */
  virtual ~ProcessCreatorBase() {}

  friend class ProcessList;
};

/**
* The template class instatiates creators for processes of a certain type.
*/
template <class T> class ProcessCreator : public ProcessCreatorBase
{
private:
  std::string name; /**< The name of the process that will be created. */

protected:
  /**
  * The function creates a process.
  * @return A pointer to the new process.
  */
  ProcessBase* create() const {return new T(name);}

public:
  /**
  * Constructor.
  * @param name The name of the process that will be created.
  */
  ProcessCreator(const std::string& name) : name(name) {}
};

/**
* The class implements a list of processes.
*/
class ProcessList : public std::list<ProcessBase*>
{
public:
  /**
  * Constructor.
  * Creates a process for each process constructor and inserts them
  * into the list.
  */
  ProcessList()
  {
    for(const ProcessCreatorBase* i = ProcessCreatorBase::first; i; i = i->next)
      push_back(i->create());
  }

  /**
  * Destructor.
  */
  ~ProcessList()
  {
    for(iterator i = begin(); i != end(); ++i)
      delete *i;
  }

  /**
  * The function announces to all processes in the list that they should stop.
  */
  void announceStop()
  {
    for(iterator i = begin(); i != end(); ++i)
      (*i)->announceStop();
  }

  /**
  * The function waits for all processes in the list to stop.
  */
  void stop()
  {
    for(iterator i = begin(); i != end(); ++i)
      (*i)->stop();
  }

  /**
  * The function starts all processes in the list.
  */
  void start()
  {
    for(iterator i = begin(); i != end(); ++i)
      (*i)->start();
  }
};

STREAMABLE(ConnectionParameter,
{
public:
  STREAMABLE(ProcessConnection,
  {,
    (std::string) sender,
    (std::string) receiver,
  }),

  (std::vector<ProcessConnection>) processConnections,
});

/**
* The macro MAKE_PROCESS instatiates a process creator.
* As a convention, it should be used in the last line of the
* source file. For each process, MAKE_PROCESS must exactly be used
* once.
* @param className The type of the class that will later be instantiated
*                 as a process.
*/
#define MAKE_PROCESS(className) \
  ProcessCreator<ProcessFrame<className> > _create##className(#className)
