#include "LogModule.h"

namespace man {
    namespace log {
        
        LogBase::LogBase(std::string lt, std::string wf) :
        logtype(lt),
        from(wf)
        {}
        
        LogBase::~LogBase()
        {}
    }
}
