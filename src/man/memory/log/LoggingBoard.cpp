
#include "LoggingBoard.h"

namespace memory {

namespace log {


LoggingBoard::LoggingBoard(const Memory* _memory) :
	memory(_memory)
{
	initLoggingObjects();
}

void LoggingBoard::initLoggingObjects() {

	const MVision* mvision = memory->getMVision();
	FDProvider* mvisionFDprovider = new FileFDProvider(NAO_LOG_DIR "/Vision.log");
	objectFDProviderMap[mvision] = mvisionFDprovider;
	objectFDLoggerMap[mvision] = new CodedFileLogger(mvisionFDprovider,
			MVISION_ID, mvision);

}

void LoggingBoard::log(const MObject* mobject) {

	ObjectFDLoggerMap::iterator it = objectFDLoggerMap.find(mobject);
	// if this is true, then we found a legitimate logger
	// corresponding to our mobject in the map
	if (it != objectFDLoggerMap.end()) {
		//it->second is the logger associated with the specified mobject
		it->second->write();
	}
}

}

}
