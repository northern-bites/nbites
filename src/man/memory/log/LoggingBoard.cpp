
#include "LoggingBoard.h"

LoggingBoard::LoggingBoard(const Memory* _memory) :
	memory(_memory)
{

	MVision* mvision = memory->getMVision();
	FDProvider* mvisionFDprovider = new FDProvider(NAO_LOG_DIR "/Vision.log");
	objectFDProviderMap[mvision] = mvisionFDprovider;
	objectFDLoggerMap[mvision] = new FDLogger(mvisionFDprovider,
			MVISION_ID, mvision);

}
