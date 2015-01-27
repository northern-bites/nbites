
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

#include "log_header.h"
#include "log_sf.h"

namespace man {
    namespace log {
        
        // Base Class
        class LogBase : public portals::Module
        {
        public:
            // The name is used as the filename
            LogBase(std::string name);
            virtual ~LogBase();
            
        protected:
            // Note that inheriting classes still need to implement this!
            virtual void run_() = 0;
            
            //encodes type and from fields usually.  Currently the only way the client/server can tell what kind of data we've sent.
            std::string description;
        };
        
        //Different types store their data different ways.
        //Further, different types need to be sent with different descriptive fields
        //If this is the case, define a specific templated logMessage for your type.
        template<typename T> inline void logMessage(T msg, std::string description) {
            std::string buffer;
            msg.SerializeToString(&buffer);
            
            nblog::NBlog(NBL_SMALL_BUFFER, 0, clock(), description.c_str(), buffer.length(), (uint8_t *) buffer.data());
            //printf("[%i]", buffer.length());
        }
        template<> inline void logMessage<messages::YUVImage>(messages::YUVImage msg, std::string description) {
            
            size_t bytes = (msg.width() * msg.height() * 1);
            
            char buf[1000];
            snprintf(buf, 1000, "%s width=%i height=%i encoding=[Y8(U8/V8)]", description.c_str(), msg.width()/2, msg.height());
            
            nblog::NBlog(NBL_IMAGE_BUFFER, 0, clock(), buf, bytes, (uint8_t *) msg.pixelAddress(0, 0));
            
            //printf("[i%i]", bytes);

        }
        
        // Template Class
        template<class T>
        class LogModule : public LogBase {
        public:
            /*
             * @brief Takes an OutPortal and wires it to this new module so that
             *        we can log its output.
             */
            LogModule(int fi, portals::OutPortal<T>* out, std::string name) : LogBase(name)
            {
                f_index = fi;
                input.wireTo(out);
            }
            
            
        protected:
            // Implements the Module run_ method
            
            //Called at the end of every diagram run.
            virtual void run_() {
                if (nbsf::flags[f_index]) {
                    input.latch();
                    logMessage<T>(input.message(), description);
                }
            }
            
            portals::InPortal<T> input;
            int f_index;
        };
        
    }
}
