
#pragma once

#include "RoboGrams.h"
#include "LogDefinitions.h"
#include "DebugConfig.h"
#include "Images.h"
#include <aio.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <list>
#include <fcntl.h>
#include <unistd.h>

#include "Logging.hpp"
#include "Control.hpp"

namespace man {
    namespace log {

        // Base Class
        class LogBase : public portals::Module
        {
        public:
            LogBase(std::string logtype, std::string wherefrom);
            virtual ~LogBase();

            std::string logtype;
            std::string from;
            
        protected:
            // Note that inheriting classes still need to implement this!
            virtual void run_() = 0;
        };
        
        //Different types store their data different ways.
        //Further, different types need to be sent with different descriptive fields
        //If this is the case, define a specific templated logMessage for your type.
        template<typename T> inline void logMessage(T msg, std::string from) {

            nbl::logptr theLog = nbl::Log::explicitLog(
                                                       std::vector<nbl::Block>{},
                                                       json::Object{},
                                                       from, time(NULL));
            theLog->addBlockFromProtobuf(msg, from, 0, clock());
            nbl::NBLog(theLog);
        }
        
        template<> inline void logMessage<messages::YUVImage>(messages::YUVImage msg, std::string from) {

            nbl::logptr theLog = nbl::Log::explicitLog(
                                                       std::vector<nbl::Block>{},
                                                       json::Object{},
                                                       from, time(NULL));
            theLog->addBlockFromImage(msg, from, 0, clock());
            nbl::NBLog(theLog);
        }
        
        // Template Class
        template<class T>
        class LogModule : public LogBase {
        public:
            /*
             * @brief Takes an OutPortal and wires it to this new module so that
             *        we can log its output.
             */
            LogModule(control::flags::flag_e fi, portals::OutPortal<T>* out, std::string lt, std::string wf) : LogBase(lt , wf)
            {
                f_index = fi;
                input.wireTo(out);
            }
            
            
        protected:
            // Implements the Module run_ method
            
            //Called at the end of every diagram run.
            virtual void run_() {
                if (control::check(f_index)) {
                    input.latch();
                    logMessage<T>(input.message(), from);
                }
            }
            
            portals::InPortal<T> input;
            control::flags::flag_e f_index;
        };
        
    }
}
