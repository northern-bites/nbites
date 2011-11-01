
#include "include/ExportDefines.h"
#include "corpus/offlineconnect/OfflineManController.h"

//This is the method loadMan's signature
typedef void (*loadManMethod)(man::corpus::OfflineManController::const_ptr);

//This is the method unloadMan's signature
typedef void (*unloadManMethod)();

START_FUNCTION_EXPORT

void loadMan(man::corpus::OfflineManController::const_ptr);

void unloadMan();

END_FUNCTION_EXPORT

#endif // MANMODULE_H
