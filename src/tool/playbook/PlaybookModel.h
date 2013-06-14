/**
 * Class responsible for holding information about a playbook.
 *
 * @author Wils Dawson
 * @date   May 2013
 */

#pragma once

#include <QObject>
#include <QStack>
#include "FieldConstants.h"
#include "PlaybookConstants.h"

namespace tool {
namespace playbook {

class PlaybookModel : public QObject
{
    Q_OBJECT;

public:
    PlaybookModel(int b_s, int g_w, int g_h, QObject* parent = 0);

    // Playbook is a big table of positions organized by:
    // 1) Goalie presence       2 possibilities
    // 2) Active Field Players  3+2+1 possibilities
    // 3) Ball X-Position       FIELD_WIDTH / BOX_SIZE + 2
    // 4) Ball Y-Position       FIELD_HEIGHT/ BOX_SIZE + 2
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
    // and 4th dimensions of the playbook, then, handle where that role should
    // go given where the ball is.

    PlaybookPosition***** playbook; // [2][3+2+1][GRID_WIDTH][GRID_HEIGHT];

    PlaybookPosition** getRobotPositions();
    int getNumActiveFieldPlayers(){return numActiveFieldPlayers;};
    int getGoalieOn(){return goalieOn;};
    short getBallX(){return ball_x;};
    short getBallY(){return ball_y;};

protected slots:
    void toggleGoalie(bool on);
    void toggleDefender(bool on);
    void toggleOffender(bool on);
    void toggleMiddie(bool on);
    void setDefenderXPosition(QString x_);
    void setDefenderYPosition(QString y_);
    void setMiddieXPosition(QString x_);
    void setMiddieYPosition(QString y_);
    void setOffenderXPosition(QString x_);
    void setOffenderYPosition(QString y_);
    void setTwoFieldPlayers(bool checked);
    void setThreeFieldPlayers(bool checked);
    void setFourFieldPlayers(bool checked);
    void setBallX(QString x_);
    void setBallY(QString y_);

protected:
    QStack<int> changedIndex;  // Holds the indecies in the playbook
                               // that were changed.
    QStack<PlaybookPosition> changedPositions; // The old positions.

    void setPosition(int value, short role, bool x_position);

    int goalieOn;
    bool defenderLocked;
    bool offenderLocked;
    bool middieLocked;
    int numActiveFieldPlayers;
    short ball_x;
    short ball_y;

    int BOX_SIZE;
    int GRID_WIDTH;
    int GRID_HEIGHT;
};

}
}
