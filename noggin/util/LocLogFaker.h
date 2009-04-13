/* LocLogFaker.h */

#ifndef LOCLOGFAKER_h_DEFINED
#define LOCLOGFAKER_h_DEFINED

#include "fakerIO.h"
#include "fakerIterators.h"

void iterateFakerPath(std::fstream * mclFile, std::fstream * ekfFile,
                      NavPath * letsGo);
#endif // LOCLOGFAKER_h_DEFINED

