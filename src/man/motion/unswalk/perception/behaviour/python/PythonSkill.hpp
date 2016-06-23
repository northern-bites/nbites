#pragma once

#include <Python.h>
#include <boost/python.hpp>
#include <sys/inotify.h>
#include <blackboard/Adapter.hpp>
#include <types/BehaviourRequest.hpp>
#include <utils/Timer.hpp>


class Blackboard;

#define INBUF_LEN 32 * (sizeof(struct inotify_event) + 16)

class PythonSkill : Adapter {
   public:
      static PythonSkill* getInstance(Blackboard *blackboard);
      PythonSkill(Blackboard *blackboard);
      ~PythonSkill();

      /* Check if Python needs a reload, and run a cycle of behaviour */
      BehaviourRequest execute();

   private:
      // used by inotify
      static PythonSkill *instance;
      int inotify_fd;
      char inotify_buf[INBUF_LEN];
      fd_set inotify_fdss;
      struct timeval inotify_timeout;
      const char *path;

      // pure python modules to import
      const char* behaviourModuleName;
      const char* robotModuleName;

      // used to make sure we clear the error before trying te execute again
      bool errorOccured;

      // refs to pure python objects
      boost::python::object main_module;
      boost::python::object sys_module;

      boost::python::object initial_modules;
      boost::python::object behaviour_module;
      boost::python::object behaviour_tick;
      boost::python::object pyKeyboardInterrupt;

      /* Start watching a directory with inotify */
      void startInotify();
      /* Start the Python interpreter, load robot and behaviour modules */
      void startPython();
      /* Reload the Python behaviour module */
      void reloadPython();
      /* Stand and say python error with LEDs flashing */
      void doErrorStance(BehaviourRequest &req);
      /* Common exception handler */
      void handlePyError(const boost::python::error_already_set &ex);
      /* Polls the inotify fd for changes that would require a reload */
      bool inotify_Check();
      /* Produce a flashy error pose. */
      ActionCommand::LED flashLEDs();
};

// global blackboard, accessible by Cython
// extern Blackboard *blackboard;
