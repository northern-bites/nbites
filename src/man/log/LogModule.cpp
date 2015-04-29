#include "LogModule.h"

namespace man {
    namespace log {
        
        LogBase::LogBase(std::string name) :
        description(name)
        {
           /* std::string img ("YUVImage");
            //isImageLogger = (description.find(img) == std::string::npos) ? false : true ;
            
            if (description.find(img) == std::string::npos) {
                isImageLogger = false;
            } else isImageLogger = true; */
        }
        
        LogBase::~LogBase()
        {
        }
    }
}
