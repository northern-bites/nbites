/*
 * Wrapper for all comm infomation needed in Python.
 *
 * @author: Wils Dawson   @date 9/17/12
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Comm.h"

void c_init_comm();

void set_comm_pointer(boost::shared_ptr<Comm> comm_ptr);
