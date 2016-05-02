
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

#include "Control.hpp"
#include "Logging.h"

using nblog::SExpr;
using nblog::NBLog;

namespace man {
    namespace log {

        // Base Class
        class LogBase : public portals::Module
        {
        public:
            LogBase(std::string logtype, std::string wherefrom);
            virtual ~LogBase();

        protected:
            // Note that inheriting classes still need to implement this!
            virtual void run_() = 0;

            std::string logtype;
            std::string from;
        };

        //Different types store their data different ways.
        //Further, different types need to be sent with different descriptive fields
        //If this is the case, define a specific templated logMessage for your type.
        template<typename T> inline void logMessage(T msg, std::string type, std::string from) {

//            std::string buffer;
//            msg.SerializeToString(&buffer);
//
//            std::vector<SExpr> sc = {
//                SExpr(type, from, clock(), -1, buffer.length())
//            };
//
//            NBLog(NBL_SMALL_BUFFER, "LogModule", sc, buffer);
        }

        template<> inline void logMessage<messages::YUVImage>(messages::YUVImage msg, std::string type, std::string from) {

//            size_t bytes = (msg.width() * msg.height() * 1);
//            std::string buffer((char *) msg.pixelAddress(0,0), bytes);
//
//            SExpr image_desc(type, from, clock(), -1, buffer.length());
//            image_desc.append(SExpr("width", msg.width() / 2));
//            image_desc.append(SExpr("height", msg.height()));
//            image_desc.append(SExpr("encoding", "[Y8(U8/V8)]"));
//
//            std::vector<SExpr> sc = {
//                image_desc
//            };
//
//            nblog::NBLog(NBL_IMAGE_BUFFER, "LogModule", sc, buffer);
        }

        // Template Class
        template<class T>
        class LogModule : public LogBase {
        public:
            /*
             * @brief Takes an OutPortal and wires it to this new module so that
             *        we can log its output.
             */
            LogModule(int fi, portals::OutPortal<T>* out, std::string lt, std::string wf) : LogBase(lt , wf)
            {
                f_index = fi;
                input.wireTo(out);
            }


        protected:
            // Implements the Module run_ method

            //Called at the end of every diagram run.
            virtual void run_() {
//                if (control::flags[f_index]) {
//                    input.latch();
//                    logMessage<T>(input.message(), logtype, from);
//                }
            }

            portals::InPortal<T> input;
            int f_index;
        };

    }
}
