/*
 * Wrapper for CommandSender.
 */

#ifndef PYCOMMANDSENDER_H
#define PYCOMMANDSENDER_H

#include "CommandSender.h"
#include <boost/shared_ptr.hpp>

void c_init_sender();

void set_sender_pointer(boost::shared_ptr<CommandSender> cs_ptr);

#endif

