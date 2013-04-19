#include "FieldViewerPainter.h"

namespace tool {
namespace viewer {

static const int PARTICLE_WIDTH = 8;

FieldViewerPainter::FieldViewerPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_),
    shouldPaintParticles(false),
    shouldPaintLocation(false),
    shouldPaintObsv(false)
{
}

void FieldViewerPainter::paintParticleAction(bool state) {

    shouldPaintParticles = state;
    repaint();
}

void FieldViewerPainter::paintLocationAction(bool state) {

    shouldPaintLocation = state;
    repaint();
}

void FieldViewerPainter::paintObsvAction(bool state) {

    shouldPaintObsv = state;
    repaint();
}

void FieldViewerPainter::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    if(shouldPaintParticles) {
        paintParticleSwarm(event, curSwarm);
    }

    if(shouldPaintLocation) {
        // Paint actual location
        paintRobotLocation(event, curLoc, true);
    }

    if(shouldPaintObsv) {
        paintObservations(event, curObsv);
    }

}

void FieldViewerPainter::paintParticleSwarm(QPaintEvent* event,
                                            messages::ParticleSwarm swarm)
{
    QPainter painter(this);

    for (int i=0; i<swarm.particle_size(); i++)
    {
        //Idea:Update scale factor based on weight? @Todo
        paintRobotLocation(event,swarm.particle(i).loc());
    }
}

QPoint FieldViewerPainter::getRelLoc(float dist, float bear)
{
    float sin, cos;
    float ninetyDeg = 1.5707963;
    sincosf((curLoc.h() + bear), &sin, &cos);

    float relX = dist*cos + curLoc.x();
    float relY = dist*sin + curLoc.y();
    QPoint relLoc(relX,relY);
    return relLoc;
}

void FieldViewerPainter::paintObservations(QPaintEvent* event,
                                           messages::VisionField obsv)
{
    QPainter painter(this);
    painter.setPen(Qt::black);

    // ToDo: paint orientation/shape
    // Corners

    for (int i=0; i<obsv.visual_corner_size(); i++) {
        if(obsv.visual_corner(i).visual_detection().on()){
                painter.setBrush(Qt::black);
                QPoint relLoc= getRelLoc(obsv.visual_corner(i).visual_detection().distance(),
                                         obsv.visual_corner(i).visual_detection().bearing());
                painter.drawEllipse(relLoc, 10, 10);
            }
    }

    if (obsv.has_goal_post_l()) {
        if (obsv.goal_post_l().visual_detection().on()) {
            painter.setBrush(Qt::yellow);
            QPoint relLoc= getRelLoc(obsv.goal_post_l().visual_detection().distance(),
                                     obsv.goal_post_l().visual_detection().bearing());
            painter.drawEllipse(relLoc, 10, 10);
        }
    }

    if (obsv.has_goal_post_r()) {
        if (obsv.goal_post_r().visual_detection().on()) {
            painter.setBrush(Qt::yellow);
            QPoint relLoc= getRelLoc(obsv.goal_post_r().visual_detection().distance(),
                                     obsv.goal_post_r().visual_detection().bearing());
            painter.drawEllipse(relLoc, 10, 10);
        }
    }

    if (obsv.has_visual_cross()) {
        if (obsv.visual_cross().on()) {
            painter.setBrush(Qt::black);
            QPoint relLoc= getRelLoc(obsv.visual_cross().distance(),
                                     obsv.visual_cross().bearing());
            painter.drawEllipse(relLoc, 10, 10);
        }
    }
}




void FieldViewerPainter::paintRobotLocation(QPaintEvent* event,
                                            messages::RobotLocation loc,
                                            bool red)
{
    QPainter painter(this);

    if (red)
        painter.setBrush(Qt::red);

    QPoint locCenter(loc.x(), loc.y());

    painter.drawEllipse(locCenter,
                        PARTICLE_WIDTH,
                        PARTICLE_WIDTH);

    painter.drawLine(loc.x(),
                     loc.y(),
                     PARTICLE_WIDTH * std::cos(loc.h()) + loc.x(),
                     PARTICLE_WIDTH * std::sin(loc.h()) + loc.y());
}

void FieldViewerPainter::updateWithLocationMessage(messages::RobotLocation newLoc)
{
    curLoc = newLoc;
    if(shouldPaintLocation) {
        repaint();
    }
}

void FieldViewerPainter::updateWithParticleMessage(messages::ParticleSwarm newSwarm)
{
    curSwarm = newSwarm;
    if(shouldPaintParticles) {
        repaint();
    }
}

void FieldViewerPainter::updateWithObsvMessage(messages::VisionField newObservations)
{
    curObsv = newObservations;
    if(shouldPaintObsv) {
        repaint();
    }
}


} // namespace viewer
} // namespace tool
