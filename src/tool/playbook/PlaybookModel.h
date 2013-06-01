/**
 * Class responsible for holding information about a playbook.
 *
 * @author Wils Dawson
 * @date   May 2013
 */

#pragma once

#include <QtObject>
#include "FieldConstants.h"

static const int BOX_SIZE = 100;  // Size in cm for each grid box.

namespace tool {
namespace playbook {

struct PlaybookPosition
{
    short x;
    short y;
    short h;
}

class PlaybookModel : QObject
{
    Q_OBJECT;

public:
    PlaybookModel(QObject* parent = 0);

protected:
    // Playbook is a big table of positions organized by:
    // 1) Goalie presence       2 possibilities
    // 2) Active Field Players  3+2+1 possibilities
    // 3) Ball Position         (FIELD_WIDTH / BOX_SIZE + 2) *
    //                          (FIELD_HIEGHT / BOX_SIZE + 2)
    // Note: #2 (above) is a priority ordered list of roles for a
    // varying number of active field players. For example, the first
    // three entries would be for when we have 4 active field players;
    // the chaser is not in the list, the first position is the
    // defender, next is the offender, and last is the middie; the
    // next two entries are for when there are 3 active field players
    // (defender and offender); and the last position is just the two
    // active field player case. So the 2nd dimension of the array is:
    //
    // 4-Defender | 4-Offender | 4-Middie | 3-Defender | 3-Offender | 2-Defender
    //
    // Where the number is the active field players present. The 3rd
    // dimension of the playbook, then, handles where that role should
    // go given where the ball is.

    PlaybookPosition playbook [2][3+2+1][(FIELD_WHITE_WIDTH/BOX_SIZE + 2)*
                                     (FIELD_WHITE_HEIGHT/BOX_SIZE +2)];

}

}
}
