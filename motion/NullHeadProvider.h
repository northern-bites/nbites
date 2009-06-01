#ifndef NullHeadProvider_h
#define NullHeadProvider_h

#include "NullProvider.h"

class NullHeadProvider : public NullProvider {
public:
    NullHeadProvider(boost::shared_ptr<Sensors> s,
					 boost::shared_ptr<Profiler> p) :
        NullProvider(s, p, MotionConstants::null_head_mask){}
    ~NullHeadProvider() {};
};

#endif
