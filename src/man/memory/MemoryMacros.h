/*
 * MemoryMacros.hpp
 *
 * This is where we define useful memory macros
 *
 *      Author: oneamtu
 */

#include "Common.h" //for micro_time

#pragma once

//TODO: should we make this do milisecs instead of
//micro to save space?
#define ADD_PROTO_TIMESTAMP extern long long int birth_time; \
        this->set_timestamp(process_micro_time() - birth_time);
