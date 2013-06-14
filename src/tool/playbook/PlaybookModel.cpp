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
    numActiveFieldPlayers(4),
    ball_x(5),
    ball_y(4)
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
    resetPositionChanges();
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

void PlaybookModel::setPosition(int value, short role, bool x_position)
{
    short roleIndex = convertRoleToPlaybookIndex(role);

    if (roleIndex != -1)
    {
        if (x_position)
            playbook[goalieOn][roleIndex][ball_x][ball_y]->x = value;
        else
            playbook[goalieOn][roleIndex][ball_x][ball_y]->y = value;

        qDebug() << "setting role: " << roleIndex << " at x? " << x_position << " to value: " << value;
    }
}

short PlaybookModel::convertRoleToPlaybookIndex(short role)
{
    short roleIndex = -1;
    if (numActiveFieldPlayers == 4)
    {
        if (role == DEFENDER)
            roleIndex = 0;
        else if (role == OFFENDER)
            roleIndex = 1;
        else if (role == MIDDIE)
            roleIndex = 2;
    }
    else if (numActiveFieldPlayers == 3)
    {
        if (role == DEFENDER)
            roleIndex = 3;
        else if (role == OFFENDER)
            roleIndex = 4;
    }
    else if (numActiveFieldPlayers == 2)
    {
        if (role == DEFENDER)
            roleIndex = 5;
    }

    return roleIndex;
}

void PlaybookModel::setDefenderXPosition(QString x_)
{
    tempDefenderX = x_.toInt();
}
void PlaybookModel::setDefenderYPosition(QString y_)
{
    tempDefenderY = y_.toInt();
}

void PlaybookModel::setMiddieXPosition(QString x_)
{
    tempMiddieX = x_.toInt();
}
void PlaybookModel::setMiddieYPosition(QString y_)
{
    tempMiddieY = y_.toInt();
}

void PlaybookModel::setOffenderXPosition(QString x_)
{
    tempOffenderX = x_.toInt();
}
void PlaybookModel::setOffenderYPosition(QString y_)
{
    tempOffenderY = y_.toInt();
}

void PlaybookModel::confirmPositionChange()
{
    setPosition(tempDefenderX, DEFENDER, true);
    setPosition(tempDefenderY, DEFENDER, false);
    setPosition(tempMiddieX, MIDDIE, true);
    setPosition(tempMiddieY, MIDDIE, false);
    setPosition(tempOffenderX, OFFENDER, true);
    setPosition(tempOffenderY, OFFENDER, false);
    qDebug() << "confirming change to a position.";
}

void PlaybookModel::resetPositionChanges()
{
    tempDefenderX = playbook[goalieOn][convertRoleToPlaybookIndex(DEFENDER)][ball_x][ball_y]->x;
    tempDefenderY = playbook[goalieOn][convertRoleToPlaybookIndex(DEFENDER)][ball_x][ball_y]->y;
    tempMiddieX = playbook[goalieOn][convertRoleToPlaybookIndex(MIDDIE)][ball_x][ball_y]->x;
    tempMiddieY = playbook[goalieOn][convertRoleToPlaybookIndex(MIDDIE)][ball_x][ball_y]->y;
    tempOffenderX = playbook[goalieOn][convertRoleToPlaybookIndex(OFFENDER)][ball_x][ball_y]->x;
    tempOffenderY = playbook[goalieOn][convertRoleToPlaybookIndex(OFFENDER)][ball_x][ball_y]->y;
    qDebug() << "resetting position fields.";
}

void PlaybookModel::confirmBallChange()
{
    ball_x = tempBallX;
    ball_y = tempBallY;
    qDebug() << "confirming change to ball's position.";
}

void PlaybookModel::resetBallChanges()
{
    tempBallX = ball_x;
    tempBallY = ball_y;
    qDebug() << "resetting ball fields.";
}

void PlaybookModel::setBallX(QString x_)
{
    tempBallX = x_.toInt();
}

void PlaybookModel::setBallY(QString y_)
{
    tempBallY = y_.toInt();
}

}
}
