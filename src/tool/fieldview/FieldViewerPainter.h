/**
 * Class responsible for drawing more things on the field, inherits from PaintField
 * so that the field itself is already drawn. Based on specs from the FieldViewer
 * Class then this class will render other images
 *
 * @author EJ Googins April 2013
 *
 */

#pragma once

#include <QtGui>
#include <vector>

#include "FieldConstants.h"

#include "common/PaintField.h"

#include "RobotLocation.pb.h"
#include "ParticleSwarm.pb.h"
#include "VisionField.pb.h"

namespace tool {
namespace viewer {

static const int PARTICLE_WIDTH = 8;

class FieldViewerPainter : public tool_common::PaintField
{
    Q_OBJECT;

public:
    FieldViewerPainter(QWidget* parent = 0, float scaleFactor_ = 1.f);

    void updateWithLocationMessage(messages::RobotLocation newLoc);
    void updateWithParticleMessage(messages::ParticleSwarm newSwarm);
    void updateWithObsvMessage(messages::VisionField newObservations);
    void updateWithOfflineMessage(messages::RobotLocation newOffline);

protected slots:
    void paintParticleAction(bool state);
    void paintLocationAction(bool state);
    void paintObsvAction(bool state);

    void handleZoomIn();
    void handleZoomOut();

protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);

    // Paint a RobotLocation
    void paintRobotLocation(QPaintEvent* event,
                            messages::RobotLocation loc,
                            bool red = false,
                            int size = PARTICLE_WIDTH);
    // Paint a Particle Swarm
    void paintParticleSwarm(QPaintEvent* event,
                            messages::ParticleSwarm swarm);
    // Paint observations
    void paintObservations(QPaintEvent* event,
                           messages::VisionField obsv);

    QPoint getRelLoc(float dist, float bear);

private:
    bool shouldPaintParticles;
    bool shouldPaintLocation;
    bool shouldPaintObsv;
    bool shouldPaintOffline;

    messages::RobotLocation curLoc;
    messages::ParticleSwarm curSwarm;
    messages::VisionField curObsv;
    messages::RobotLocation curOffline;
};

} // namespace viewer
} // namespace tool
