#include "PythonSkill.hpp"
#include <iostream>
#include <stdexcept>
#include <boost/regex.hpp>
#include <signal.h>
#include "blackboard/Blackboard.hpp"
#include "utils/speech.hpp"
#include "thread/ThreadManager.hpp"

#include <soccer.hpp>

#define SWIG_as_voidptr(a) const_cast< void * >(static_cast< const void * >(a))

extern "C"
void initrobot(void);

using namespace std;
using namespace boost::python;

/* All Py_* functions return new references unless otherwise specififed */

PythonSkill::PythonSkill(Blackboard *bb) : Adapter(bb) {
   path                = blackboard->config["behaviour.path" ].as<string>().c_str();
   behaviourModuleName = "behaviour";

   behaviour_module = object();
   behaviour_tick   = object();

   robotModuleName = "robot";

   startInotify();
   startPython();
}

void PythonSkill::startInotify() {
   inotify_fd = inotify_init();
   int wd;
   wd  = inotify_add_watch(inotify_fd, path,
                           IN_MODIFY | IN_ATTRIB | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE);
   if (wd < 0) {
      llog(ERROR) << "Failed to start watching direcotry: " << path << endl;
   }
   wd = inotify_add_watch(inotify_fd, (string(path) + "/skills").c_str(),
                          IN_MODIFY | IN_ATTRIB | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE);
   if (wd < 0) {
      llog(ERROR) << "Failed to start watching direcotry: " << path << endl;
   }
   inotify_timeout.tv_sec = 0;
   inotify_timeout.tv_usec = 0;
}

void PythonSkill::startPython() {
   errorOccured = false;

   // Kill any stray interpreters
   if (Py_IsInitialized()) {
      Py_Finalize();
   }

   // Start interpreter
   Py_Initialize();

   // This try block is peculiar to the interpreter initialisation
   try {
      // Load robotmodule C extension module
      initrobot();

      // Get handle to the special main module and sys
      main_module = import("__main__");
      sys_module  = import("sys");

      // Add behaviour.path option value to python path
      object sys_path = sys_module.attr("path");
      sys_path.attr("append")(path);

      // Obtain KeyboardInterrupt exception
      object exceptions_module = import("exceptions");
      pyKeyboardInterrupt      = exceptions_module.attr("KeyboardInterrupt");

      // Import robot
      object robotModule = import(robotModuleName);

   } catch (const error_already_set &ex) {
      handlePyError(ex);

      // If the initialisation fails, we are unable to handle the error
      llog(ERROR) << "Python failed to start." << std::endl;
      std::cout   << "Python failed to start. Shutting Down." << std::endl;

      // Rethrowing the exception will simply result in perception restarting.
      // Init shutdown instead.
      attemptingShutdown = true;
      return;
   }

   // Try to load the behaviour module. Again, not much we
   // can do if this fails.
   try {
      behaviour_module = import(behaviourModuleName);

      // Used when we "reinitialise" the python interpreter on behaviour reload
      initial_modules = extract<dict>(sys_module.attr("modules"))().copy();

   } catch (const error_already_set &ex) {
      handlePyError(ex);

      // If the initialisation fails, we are unable to handle the error
      llog(ERROR) << "Could not load module " << behaviourModuleName
                  << "." << std::endl;

      std::cout   << "Could not load module " << behaviourModuleName
                  << ". Shutting Down." << std::endl;

      attemptingShutdown = true;
      return;
   }

   // From this point it is possible to reload the behaviour in the
   // event of failure.
   try {
      // Obtain tick() function
      behaviour_tick = behaviour_module.attr("tick");
   } catch (const error_already_set &ex) {
      handlePyError(ex);
   }
}

void PythonSkill::reloadPython()
{
   /* Clear previously loaded modules */
   dict modules = extract<dict>(sys_module.attr("modules"));
   modules.clear();
   modules.update(initial_modules);

   /* Reloading cannot be done through the boost interface.
    *
    * Assigning to the behaviourModule will automatically
    * decrement the reference count on the old behaviour module.
    *
    * Warning: ReloadModule() will leak memory.
    * See bugs.python.org/issue9072
    */
   PyObject *new_behaviour_module = PyImport_ReloadModule(behaviour_module.ptr());
   /* Using the C api will not raise exceptions, so raise one manually on error */
   if (PyErr_Occurred()) {
      throw error_already_set();
   }

   behaviour_module = object(handle<>(new_behaviour_module));
   behaviour_tick   = behaviour_module.attr("tick");
}


PythonSkill::~PythonSkill() {
   Py_Finalize(); // frees all interpreter memory
}

BehaviourRequest PythonSkill::execute() {
   BehaviourRequest req;
   try {
      bool reloadNeeded = inotify_Check();
      if (reloadNeeded || errorOccured) { // TODO(davidc) in games add || pythonError
         reloadPython();
         errorOccured = false;
      } else if (! errorOccured) {
         object bb(*blackboard);
         object py_br = behaviour_tick(bb);
         req = extract<BehaviourRequest>(py_br);
      } else {
         doErrorStance(req);
      }
   } catch (const error_already_set &ex) {
      handlePyError(ex);
      doErrorStance(req);
   } catch (const std::exception& ex) {
      // This is the worst thing ever and I apologize to the high council of correct logging but this bug is really
      // intermittent but kills us when it happens so we need it to log even if it's in game (it's not like the robot
      // will be continuing from this point anyway so we're not slowing things down). I hereby break the log of silence.
      // - Ritwik
      llog(SILENT) << "Caught wild PythonSkill execute() exception." << std::endl;
      throw ex;
   }

   return req;
}

void PythonSkill::doErrorStance(BehaviourRequest &req)
{
   static struct timeval lastSaid = {0, 0};

   struct timeval now, diff;
   gettimeofday(&now, NULL);
   timersub(&now, &lastSaid, &diff);

   if (diff.tv_sec > 10) {
      lastSaid = now;
      SAY("Python error");
   }

   req.actions.body.actionType = ActionCommand::Body::STAND;
   req.actions.leds = flashLEDs();
}

ActionCommand::LED PythonSkill::flashLEDs() {

   const ActionCommand::rgb err(1, 0, 1);
   const ActionCommand::rgb off(0, 0, 0);

   struct timeval tv;
   gettimeofday(&tv, NULL);

   if ((tv.tv_usec / 250000) % 2) {
      return ActionCommand::LED(err, err, err, err, err);
   } else {
      return ActionCommand::LED(off, off, off, off, off);
   }
}

bool PythonSkill::inotify_Check() {
   bool reloadNeeded = false;
   FD_ZERO(&inotify_fdss);
   FD_SET(inotify_fd, &inotify_fdss);
   int selret = select(inotify_fd + 1, &inotify_fdss, NULL, NULL, &inotify_timeout);
   int i, len;
   if (selret < 0) {
      cout << "select on inotify fd failed";
   } else if (selret && FD_ISSET(inotify_fd, &inotify_fdss)) {
      /* inotify event(s) available! */
      i = 0;
      len = read(inotify_fd, inotify_buf, INBUF_LEN);
      if (len < 0) {
         cout << "read on inotify fd failed" << endl;
      } else if (len) {
         while (i < len) {
            struct inotify_event *event;
            event = (struct inotify_event *) &inotify_buf[i];
            if (event->len) {

               // This is a hack to get file reloading working on python files within directories.
               // I'm not sure on the proper solution, so until someone figures it out, this will
               // probably stay here.  

               // boost::regex matchRegex(".*\\.py$");
               // if (regex_match(event->name, matchRegex)) {
                  cout << "Detected change in " << event->name << endl;
                  reloadNeeded = true;
                  break;
               // }
            }
            i += sizeof(struct inotify_event) + event->len;
         }
      }
   }
   return reloadNeeded;
}

void PythonSkill::handlePyError(const error_already_set &ex) {
   PyObject *pyException = PyErr_Occurred();
   if (pyException != NULL) {
      if (PyErr_GivenExceptionMatches(pyException, pyKeyboardInterrupt.ptr())) {
         handleSignals(SIGINT, NULL, NULL);
      } else {
         errorOccured = true;
      }
      PyErr_Print();
      if (Py_FlushLine()) {
         PyErr_Clear();
      }
   } else {
      /* We received an exception but PyErr was clear. Since we don't know what
       * to do, throw the exception and likely cause the perception thead to be
       * restarted.
       */
      throw ex;
   }
}

PythonSkill *PythonSkill::getInstance(Blackboard *blackboard) {
   if (PythonSkill::instance == NULL) {
      PythonSkill::instance = new PythonSkill(blackboard);
   }
   return instance;
}

PythonSkill* PythonSkill::instance = NULL;
