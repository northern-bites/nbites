/**
 * @class OfflineEnactor
 *
 * offline enactor faker
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include "corpus/MotionEnactor.h"

namespace man {
namespace corpus {

class OfflineEnactor : public MotionEnactor {

public:
	OfflineEnactor() {}

	void sendCommands() {}
	void postSensors() {}

};

}
}
