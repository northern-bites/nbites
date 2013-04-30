#include "FieldViewerPainter.h"

namespace tool {
namespace viewer {



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

    if(shouldPaintParticles)
        paintParticleSwarm(event, curSwarm);

    if(shouldPaintLocation)
        paintRobotLocation(event, curLoc, true);

    if(shouldPaintObsv)
        paintObservations(event, curObsv);
}

void FieldViewerPainter::paintParticleSwarm(QPaintEvent* event,
                                            messages::ParticleSwarm swarm)
{
    QPainter painter(this);
    //Move origin to bottem left and scale to flip the y axis
    painter.translate(0,FIELD_GREEN_HEIGHT*scaleFactor);
    painter.scale(scaleFactor, -scaleFactor);

    for (int i=0; i<swarm.particle_size(); i++)
    {
        float size = swarm.particle(i).weight() * 50 *PARTICLE_WIDTH;
        //Idea:Update scale factor based on weight? @Todo
        paintRobotLocation(event,swarm.particle(i).loc(),
                           false, size);
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
    //Move origin to bottem left and scale to flip the y axis
    painter.translate(0,FIELD_GREEN_HEIGHT*scaleFactor);
    painter.scale(scaleFactor, -scaleFactor);
    painter.setPen(Qt::black);

    // ToDo: paint orientation/shape
    // Corners

    for (int i=0; i<obsv.visual_corner_size(); i++) {
        if(obsv.visual_corner(i).visual_detection().distance() > 0.f){
                painter.setBrush(Qt::black);
                QPoint relLoc= getRelLoc(obsv.visual_corner(i).visual_detection().distance(),
                                         obsv.visual_corner(i).visual_detection().bearing());
                painter.drawEllipse(relLoc, 10, 10);

                // Paint the possible locations in purple and tiny
                for (int j=0; j<obsv.visual_corner(i).visual_detection().concrete_coords_size(); j++)
                {
                    float concX = obsv.visual_corner(i).visual_detection().concrete_coords(j).x();
                    float concY = obsv.visual_corner(i).visual_detection().concrete_coords(j).y();
                    QPoint relCoord(concX, concY);
                    painter.setBrush(Qt::magenta);
                    painter.drawEllipse(relCoord,5,5);
                    painter.setBrush(Qt::black);
                }
            }
    }

    if (obsv.has_goal_post_l()) {
        if (obsv.goal_post_l().visual_detection().on()
           && (obsv.goal_post_l().visual_detection().distance() > 0.f)){
            painter.setBrush(Qt::yellow);
            QPoint relLoc= getRelLoc(obsv.goal_post_l().visual_detection().distance(),
                                     obsv.goal_post_l().visual_detection().bearing());
            painter.drawEllipse(relLoc, 10, 10);

            for (int j=0; j<obsv.goal_post_l().visual_detection().concrete_coords_size(); j++)
            {
                float concX = obsv.goal_post_l().visual_detection().concrete_coords(j).x();
                float concY = obsv.goal_post_l().visual_detection().concrete_coords(j).y();
                QPoint relCoord(concX, concY);
                painter.setBrush(Qt::magenta);
                painter.drawEllipse(relCoord,5,5);
            }
        }
    }

    if (obsv.has_goal_post_r()) {
        if (obsv.goal_post_r().visual_detection().on()
           && (obsv.goal_post_r().visual_detection().distance() > 0.f)){
            painter.setBrush(Qt::red);
            QPoint relLoc= getRelLoc(obsv.goal_post_r().visual_detection().distance(),
                                     obsv.goal_post_r().visual_detection().bearing());
            painter.drawEllipse(relLoc, 10, 10);
            for (int j=0; j<obsv.goal_post_r().visual_detection().concrete_coords_size(); j++)
            {
                float concX = obsv.goal_post_r().visual_detection().concrete_coords(j).x();
                float concY = obsv.goal_post_r().visual_detection().concrete_coords(j).y();
                QPoint relCoord(concX, concY);
                painter.setBrush(Qt::magenta);
                painter.drawEllipse(relCoord,5,5);
            }
        }
    }

    if (obsv.has_visual_cross()) {
        if (obsv.visual_cross().distance() > 0.f){
            painter.setBrush(Qt::black);
            QPoint relLoc= getRelLoc(obsv.visual_cross().distance(),
                                     obsv.visual_cross().bearing());
            painter.drawEllipse(relLoc, 10, 10);

            for (int j=0; j<obsv.visual_cross().concrete_coords_size(); j++)
            {
                float concX = obsv.visual_cross().concrete_coords(j).x();
                float concY = obsv.visual_cross().concrete_coords(j).y();

                QPoint relCoord(concX, concY);
                painter.setBrush(Qt::magenta);
                painter.drawEllipse(relCoord,5,5);
            }
        }
    }
}




void FieldViewerPainter::paintRobotLocation(QPaintEvent* event,
                                            messages::RobotLocation loc,
                                            bool red,
                                            int size)
{
    QPainter painter(this);
    //Move origin to bottem left and scale to flip the y axis
    painter.translate(0,FIELD_GREEN_HEIGHT*scaleFactor);
    painter.scale(scaleFactor, -scaleFactor);

    if (red)
        painter.setBrush(Qt::red);

    QPoint locCenter(loc.x(), loc.y());

    painter.drawEllipse(locCenter,
                        size,
                        size);

    painter.drawLine(loc.x(),
                     loc.y(),
                     size * std::cos(loc.h()) + loc.x(),
                     size * std::sin(loc.h()) + loc.y());
}

void FieldViewerPainter::handleZoomIn()
{
    scaleFactor += .1;
    repaint();
}

void FieldViewerPainter::handleZoomOut()
{
    qDebug() << "Zoom out";

    scaleFactor -= .1;
    repaint();
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
