#ifndef NullBodyProvider_h
#define NullBodyProvider_h

#include "NullProvider.h"

namespace man
{
namespace motion
{
class NullBodyProvider : public NullProvider 
{
public:
    NullBodyProvider() :
        NullProvider(MotionConstants::null_body_mask){}
    virtual ~NullBodyProvider(){};
};
} // namespace motion
} // namespace man

#endif
