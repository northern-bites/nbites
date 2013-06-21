/**
 * @brief A class responsible for maintaing knowedge of lines on the field
 *        and comparing projectinos and observations
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   June 2013
 */

#pragma once

#include "FieldConstants.h"
#include "LocStructs.h"

namespace man {
namespace localization {

typedef std::vector<Line> LineSet;
typedef LineSet::iterator LineIt;

class LineSystem {
public:
    LineSystem();
    virtual ~LineSystem();

    void addLine(float startX, float startY, float endX, float endY);

    float scoreObservation(Line globalObsv);
    LineErrorMatch scoreAndMatchObservation(Line globalObsv, bool debug = false);

private:
    LineSet lines;
};

} // namespace localization
} // namespace man
