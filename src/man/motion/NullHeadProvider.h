#ifndef NullHeadProvider_h
#define NullHeadProvider_h

#include "NullProvider.h"

class NullHeadProvider : public NullProvider {
public:
    NullHeadProvider(boost::shared_ptr<Sensors> s) :
        NullProvider(s, MotionConstants::null_head_mask){}
    virtual ~NullHeadProvider() {};
};

#endif
