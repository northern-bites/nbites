/**
 * OfflineManLoader
 *
 * creates an instance of Man with all Offline robot connect interfaces
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include "include/ExportDefines.h"
#include "corpus/offlineconnect/OfflineManController.h"

//This is the method loadMan's signature
typedef void (*loadManMethod)(man::corpus::OfflineManController::ptr);

//This is the method unloadMan's signature
typedef void (*unloadManMethod)(man::corpus::OfflineManController::ptr);

START_FUNCTION_EXPORT

void loadMan(man::corpus::OfflineManController::ptr);

void unloadMan(man::corpus::OfflineManController::ptr);

END_FUNCTION_EXPORT
