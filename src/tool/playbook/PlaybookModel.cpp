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
    chaserLocked(false),
    BOX_SIZE(b_s),
    GRID_WIDTH(g_w),
    GRID_HEIGHT(g_h),
    numActiveFieldPlayers(4),
    ball_x(4),
    ball_y(4)
{
    playbook = new PlaybookPosition****[2];
    for(int goalie = 0; goalie < 2; ++goalie)
    {
        playbook[goalie] = new PlaybookPosition***[GRID_WIDTH];
        for(int x = 0; x < GRID_WIDTH; ++x)
        {
            playbook[goalie][x] = new PlaybookPosition**[GRID_HEIGHT];
            for(int y = 0; y < GRID_HEIGHT; ++y)
            {
                playbook[goalie][x][y] = new PlaybookPosition*[4+3+2+1];
                for(int role = 0; role < 4+3+2+1; ++role)
                {
                    PlaybookPosition* p = new PlaybookPosition(
                        LANDMARK_BLUE_GOAL_CROSS_X,
                        LANDMARK_BLUE_GOAL_CROSS_Y + (role%4 - 1) * CENTER_CIRCLE_RADIUS,
                        0,
                        defaultRoleList[role]);
                    playbook[goalie][x][y][role] = p;
                }
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

void PlaybookModel::toggleChaser(bool on)
{
    chaserLocked = on ? 1 : 0;
    qDebug() << "chaserLocked is now " << chaserLocked;
}

void PlaybookModel::toggleMiddie(bool on)
{
    middieLocked = on ? 1 : 0;
    qDebug() << "middieLocked is now " << middieLocked;
}

// Not used.
void PlaybookModel::setPosition(int value, short role, bool x_position)
{
    return;
}

PlaybookPosition* PlaybookModel::convertRoleToPlaybookPosition(short role)
{
    int offset = 0;
    if (numActiveFieldPlayers == 4) {
        offset = 0;
    } else if (numActiveFieldPlayers == 3) {
        offset = 4;
    } else if (numActiveFieldPlayers == 2) {
        offset = 7;
    } else if (numActiveFieldPlayers == 1) {
        offset = 9;
    }

    PlaybookPosition* position;
    for (int i = 0; i < numActiveFieldPlayers; i++)
    {
        position = playbook[goalieOn][ball_x][ball_y][i+offset];
        if (position->role == role)
        {
            return position;
        }
    }

    qDebug() << "gave an invalid role to the conversion method!";
    return playbook[0][0][0][0];
}

void PlaybookModel::setDefenderXPosition(int x_)
{
    convertRoleToPlaybookPosition(DEFENDER)->x = x_;
}
void PlaybookModel::setDefenderYPosition(int y_)
{
    convertRoleToPlaybookPosition(DEFENDER)->y = y_;
}
void PlaybookModel::setDefenderHPosition(int h_)
{
    convertRoleToPlaybookPosition(DEFENDER)->h = h_;
}

void PlaybookModel::setMiddieXPosition(int x_)
{
    convertRoleToPlaybookPosition(MIDDIE)->x = x_;
}
void PlaybookModel::setMiddieYPosition(int y_)
{
    convertRoleToPlaybookPosition(MIDDIE)->y = y_;
}
void PlaybookModel::setMiddieHPosition(int h_)
{
    convertRoleToPlaybookPosition(MIDDIE)->h = h_;
}

void PlaybookModel::setOffenderXPosition(int x_)
{
    convertRoleToPlaybookPosition(OFFENDER)->x = x_;
}
void PlaybookModel::setOffenderYPosition(int y_)
{
    convertRoleToPlaybookPosition(OFFENDER)->y = y_;
}
void PlaybookModel::setOffenderHPosition(int h_)
{
    convertRoleToPlaybookPosition(OFFENDER)->h = h_;
}

void PlaybookModel::setChaserXPosition(int x_)
{
    convertRoleToPlaybookPosition(CHASER)->x = x_;
}
void PlaybookModel::setChaserYPosition(int y_)
{
    convertRoleToPlaybookPosition(CHASER)->y = y_;
}
void PlaybookModel::setChaserHPosition(int h_)
{
    convertRoleToPlaybookPosition(CHASER)->h = h_;
}

void PlaybookModel::setBallX(int x_)
{
    ball_x = x_;
    qDebug() << "setting the ball's x position to: " << x_;
}

void PlaybookModel::setBallY(int y_)
{
    ball_y = y_;
    qDebug() << "setting the ball's y position to: " << y_;
}

}
}
