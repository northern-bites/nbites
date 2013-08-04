#pragma once

#include "NullProvider.h"

namespace man
{
namespace motion
{

class NullHeadProvider : public NullProvider
{
public:
    NullHeadProvider()
        : NullProvider(MotionConstants::null_head_mask)
        {
        }

    virtual ~NullHeadProvider()
    {
    };
};

} // namespace motion
} // namespace man
