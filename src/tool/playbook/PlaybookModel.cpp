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
    GRID_HEIGHT(g_h),
    numActiveFieldPlayers(4)
{
    playbook = new PlaybookPosition****[2];
    for(int goalie = 0; goalie < 2; ++goalie)
    {
        playbook[goalie] = new PlaybookPosition***[3+2+1];
        for(int role = 0; role < 3+2+1; ++role)
        {
            playbook[goalie][role] = new PlaybookPosition**[GRID_WIDTH];
            for(int x = 0; x < GRID_WIDTH; ++x)
            {
                playbook[goalie][role][x] = new PlaybookPosition*[GRID_HEIGHT];
                for(int y = 0; y < GRID_HEIGHT; ++y)
                {
                    PlaybookPosition* p = new PlaybookPosition(
                        LANDMARK_BLUE_GOAL_CROSS_X,
                        LANDMARK_BLUE_GOAL_CROSS_Y + (role%3 - 1) * CENTER_CIRCLE_RADIUS,
                        0,
                        defaultRoleList[role]);
                    playbook[goalie][role][x][y] = p;
                }
            }
        }
    }
}

PlaybookPosition** PlaybookModel::getRobotPositions()
{
    PlaybookPosition** positions = new PlaybookPosition*[3];
    for (int i = 0; i < 3; i++)
    {
        positions[i] = playbook[0][i][0][0];
    }
    return positions;
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

void PlaybookModel::setTwoFieldPlayers(bool checked)
{
    if (checked)
    {
        numActiveFieldPlayers = 2;
        qDebug() << "number of active field players is now 2";
    }
}

void PlaybookModel::setThreeFieldPlayers(bool checked)
{
    if (checked)
    {
        numActiveFieldPlayers = 3;
        qDebug() << "number of active field players is now 3";
    }
}

void PlaybookModel::setFourFieldPlayers(bool checked)
{
    if (checked)
    {
        numActiveFieldPlayers = 4;
        qDebug() << "number of active field players is now 4";
    }
}

void PlaybookModel::setDefenderXPosition(QString x_)
{
    playbook[0][DEFENDER][0][0]->x = x_.toInt();
    qDebug() << "setting defender's position: x value is now " << x_;
}

void PlaybookModel::setDefenderYPosition(QString y_)
{
    playbook[0][DEFENDER][0][0]->y = y_.toInt();
    qDebug() << "setting defender's position: y value is now " << y_;
}
void PlaybookModel::setMiddieXPosition(QString x_)
{
    playbook[0][MIDDIE][0][0]->x = x_.toInt();
    qDebug() << "setting middie's position: x value is now " << x_;
}

void PlaybookModel::setMiddieYPosition(QString y_)
{
    playbook[0][MIDDIE][0][0]->y = y_.toInt();
    qDebug() << "setting middie's position: y value is now " << y_;
}
void PlaybookModel::setOffenderXPosition(QString x_)
{
    playbook[0][OFFENDER][0][0]->x = x_.toInt();
    qDebug() << "setting offender's position: x value is now " << x_;
}

void PlaybookModel::setOffenderYPosition(QString y_)
{
    playbook[0][OFFENDER][0][0]->y = y_.toInt();
    qDebug() << "setting offender's position: y value is now " << y_;
}

}
}
