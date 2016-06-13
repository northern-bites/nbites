//
//  Filesystem.hpp
//  tool8-separate
//
//  Created by Philip Koch on 4/7/16.
//

#ifndef Filesystem_hpp
#define Filesystem_hpp

#include <stdio.h>
#include "Logging.hpp"

namespace nbl {
    namespace filesystem {

        /* 
         fileio thread is typically almost completely unused compared to streaming
         this field determines how frequently the thread checks the control::flags::fileio flag
         ** may be removed in future if filesystem logging increases **
        */
        static const unsigned FILE_THREAD_SLEEP_SECS = 5;

        class Filelogger : public Threadable {
            LogProvider& provider;
        public:
            Filelogger(LogProvider& p) : Threadable(),
                provider(p) {}
            void threadLoop();
        };

    }
}

#endif /* Filesystem_hpp */
