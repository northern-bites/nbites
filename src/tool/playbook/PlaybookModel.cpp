#include "PlaybookModel.h"

#include <QtDebug>

namespace tool{
namespace playbook{

PlaybookModel::PlaybookModel(QObject* parent) :
    QObject(parent)
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


}
}
