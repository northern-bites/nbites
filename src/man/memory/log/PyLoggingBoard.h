/**
 * Boost python wrapping for the logging board
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 * @author Lizzie Mamantov <emamanto@bowdoin.edu>
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "LoggingBoard.h"

namespace man {
namespace memory {
namespace log {

void c_init_logging();

void set_logging_board_pointer(boost::shared_ptr<LoggingBoard> logging_board_pointer);

}
}
}
