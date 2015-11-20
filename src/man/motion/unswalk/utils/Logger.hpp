#pragma once

#include <string>
#include <ostream>
#include <boost/program_options/variables_map.hpp>

#define llog(X) ((Logger::instance())->realLlog(X))

/**
 * Possible log levels
 * When something is to be logged an associated level is passed with it. Only
 * messages with levels greater than logLevel are actually logged at runtime.
 * Levels should be set to an appropriate point, so that selecting a logLevel
 * such as INFO gives a corresponding level of logging.
 * A message is FATAL if the robot cannot function in its current state.
 * An ERROR, while still allowing a robot to run, greatly impairs its ability
 * to function accurately or as intended.
 * INFO gives useful, concise summaries of the robot's functioning.
 * VERBOSE details at length every step that INFO summarises.
 * DEBUGx gives multiple levels of debugging, for the coder's pleasure.
 * @see logLevel
 */
enum LogLevel {
   NONE  = -200,
   SILENT  = -100,
   QUIET   = -67,
   FATAL   = -33,
   ERROR   = 0,
   WARNING = 10,
   INFO    = 20,
   VERBOSE = 40,
   /*DEBUG   = 60,*/
   DEBUG1  = 60,
   DEBUG2  = 80,
   DEBUG3  = 100
};

class Logger {
   public:
      Logger(const char *name);
      virtual ~Logger();
      static void init(std::string logPath, std::string logLevel, bool motion);
      static Logger *instance();
      std::ostream &realLlog(int logLevel);

   private:
      static void readOptions(const boost::program_options::variables_map &config);
      static void init(std::string logLevel, bool motion);
      static __thread Logger *logger;
      static enum LogLevel logLevel;
      static bool motion;
      static std::string logPath;
      static bool initialised;
      std::ostream *logStream;
};
