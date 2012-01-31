/*
 * Wrapper for CommandSender.
 *
 * @author Lizzie Mamantov
 */

#ifndef PYCOMMANDSENDER_H
#define PYCOMMANDSENDER_H

#include "CommandSender.h"
#include <boost/shared_ptr.hpp>

// For initializing the module, in Noggin
void c_init_sender();

// For setting a ptr to the actual C++ CommandSender
void set_sender_pointer(boost::shared_ptr<CommandSender> cs_ptr);

#endif

