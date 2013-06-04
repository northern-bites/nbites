#include "PlaybookModel.h"

#include <QtDebug>

namespace tool{
namespace playbook{

PlaybookModel::PlaybookModel(int b_s, int g_w, int g_h, QObject* parent) :
    QObject(parent),
    changedIndex(),
    changedPositions(),
    goalieOn(1),
    defenderLocked(false),
    offenderLocked(false),
    middieLocked(false),
    BOX_SIZE(b_s),
    GRID_WIDTH(g_w),
    GRID_HEIGHT(g_h)
{
    playbook = new PlaybookPosition***[2];
    for(int goalie = 0; goalie < 2; ++goalie)
    {
        playbook[goalie] = new PlaybookPosition**[3+2+1];
        for(int role = 0; role < 3+2+1; ++role)
        {
            playbook[goalie][role] = new PlaybookPosition*[GRID_WIDTH];
            for(int x = 0; x < GRID_WIDTH; ++x)
            {
                playbook[goalie][role][x] = new PlaybookPosition(
                    LANDMARK_BLUE_GOAL_CROSS_X,
                    LANDMARK_BLUE_GOAL_CROSS_Y + (role%3 - 1) * CENTER_CIRCLE_RADIUS,
                    0);
            }
        }
    }
}

void PlaybookModel::toggleGoalie(bool on)
{
    goalieOn = on ? 1 : 0;
    qDebug() << "goalieOn is now " << goalieOn;
}

void PlaybookModel::toggleDefender(bool on)
{
    defenderLocked = on ? 1 : 0;
    qDebug() << "defenderLocked is now " << defenderLocked;
}

void PlaybookModel::toggleOffender(bool on)
{
    offenderLocked = on ? 1 : 0;
    qDebug() << "offenderLocked is now " << offenderLocked;
}

void PlaybookModel::toggleMiddie(bool on)
{
    middieLocked = on ? 1 : 0;
    qDebug() << "middieLocked is now " << middieLocked;
}

}
}
