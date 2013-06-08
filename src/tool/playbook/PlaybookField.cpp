#include "PlaybookField.h"

#include <QtDebug>

namespace tool {
namespace playbook {

PlaybookField::PlaybookField(int b_s, int g_w, int g_h, PlaybookModel* m,
                             QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_),
    model(m),
    shouldPaintGoalie(true),
    BOX_SIZE(b_s),
    GRID_WIDTH(g_w),
    GRID_HEIGHT(g_h)
{
    robots = model->getRobotPositions();
}

void PlaybookField::drawGoalie(bool on)
{
    shouldPaintGoalie = on;
    //qDebug() << "Drawing goalie " << on;
    update();
}

void PlaybookField::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    paintGrid(event);
    paintPlayers(event);

    if(shouldPaintGoalie)
    {
        paintGoalie(event);
    }
}

void PlaybookField::paintGrid(QPaintEvent* event)
{
    QPainter painter(this);
    //Move orign to bottom left and scale to flip y axis
    painter.translate(0,FIELD_GREEN_HEIGHT);
    painter.scale(scaleFactor, -scaleFactor);

    painter.setBrush(Qt::black);

    //Draw Vertical Lines
    for (int i = FIELD_WHITE_LEFT_SIDELINE_X;
         i < FIELD_GREEN_RIGHT_SIDELINE_X;
         i += BOX_SIZE)
    {
        painter.drawLine(i, 0, i, FIELD_HEIGHT);
    }

    //Draw Horizontal Lines
    for (int i = FIELD_WHITE_BOTTOM_SIDELINE_Y; i < FIELD_HEIGHT; i += BOX_SIZE)
    {
        painter.drawLine(0, i, FIELD_WIDTH, i);
    }

}

void PlaybookField::paintGoalie(QPaintEvent* event)
{
    QPainter painter(this);
    //Move orign to bottom left and scale to flip y axis
    painter.translate(0,FIELD_GREEN_HEIGHT);
    painter.scale(scaleFactor, -scaleFactor);

    paintRobot(event, painter,
               FIELD_WHITE_LEFT_SIDELINE_X + 15, CENTER_FIELD_Y, 0, //TODO: don't hard code these
               roleColors[GOALIE]);
}

void PlaybookField::paintPlayers(QPaintEvent* event)
{
    QPainter painter(this);
    //Move origin to bottom left and scale to flip y axis
    painter.translate(0,FIELD_GREEN_HEIGHT);
    painter.scale(scaleFactor, -scaleFactor);

    // Get robot positions.
    robots = model->getRobotPositions();

    // Paint each robot.
    for (int i = 0; i < 3; i++) //TODO: use constant, adjust based on number of active field players
    {
        char role = robots[i]->role;
        int roleNum = 0;
        if (role == 'd')
        {
            roleNum = 0;
        }
        else if (role == 'm')
        {
            roleNum = 1;
        }
        else if (role == 'o')
        {
            roleNum = 2;
        }

        paintRobot(event, painter,
                   robots[i]->x,
                   robots[i]->y,
                   robots[i]->h,
                   roleColors[roleNum]);
    }
}

void PlaybookField::paintRobot(QPaintEvent* event,
                               QPainter& painter,
                               int x, int y, int h,
                               Qt::GlobalColor c,
                               float sizeX, float sizeY)
{
    painter.save();

    painter.setBrush(c);

    painter.translate(x,y);
    painter.rotate(h);

    QPoint center(0, 0);
    QRect robot(0, 0, sizeX*scaleFactor, sizeY*scaleFactor);
    QPoint lineEnd(0.125*x*scaleFactor,
                   0);

    robot.moveCenter(center);

    painter.drawRect(robot);
    painter.drawLine(center, lineEnd);

    painter.restore();
}

} // namespace playbook
} // namespace tool
