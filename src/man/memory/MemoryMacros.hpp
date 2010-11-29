/*
 * MemoryMacros.hpp
 *
 * This is where we define all the memory macros we use to
 * link objects to their corresponding memory objects
 *
 *      Author: oneamtu
 */

#pragma once

//macros for memory "linking"
#define memory_link_objects(src,dest) \
    bool src::updateMemory(dest obj)
#define memory_link(src,dest) \
    set_dest(obj.src);
