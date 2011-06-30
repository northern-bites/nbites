#ifndef NullBodyProvider_h
#define NullBodyProvider_h

#include "NullProvider.h"

class NullBodyProvider : public NullProvider {
public:
    NullBodyProvider(boost::shared_ptr<Sensors> s) :
        NullProvider(s, MotionConstants::null_body_mask){}
    virtual ~NullBodyProvider(){};
};

#endif
