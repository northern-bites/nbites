#pragma once

#include <stdint.h>
#include <string>
#include <sstream>

namespace messages {

	class LedCommand {
	public:
		LedCommand() : {}

		long long get_timestamp() const { return timestamp; }
		void set_timestamp(long long t) { timestamp = t; }
		void Clear() { timestamp = 0; }

		long long timestamp;
	};
}
