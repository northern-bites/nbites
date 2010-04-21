#ifndef NullBodyProvider_h
#define NullBodyProvider_h

#include "NullProvider.h"

class NullBodyProvider : public NullProvider {
public:
    NullBodyProvider(boost::shared_ptr<Sensors> s,
					 boost::shared_ptr<Profiler> p) :
        NullProvider(s, p, MotionConstants::null_body_mask){}
    ~NullBodyProvider(){};
};

#endif
