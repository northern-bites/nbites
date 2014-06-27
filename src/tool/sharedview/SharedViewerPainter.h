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

#include "localization/VisionSystem.h" //keep up to date with how lines are segmented
#include "localization/Particle.h"
#include "localization/LineSystem.h"

#include "FieldConstants.h"
#include "Common.h"

#include "common/PaintField.h"

#include "RobotLocation.pb.h"
#include "ParticleSwarm.pb.h"
#include "VisionField.pb.h"

namespace tool {
namespace sharer {

static const float MIN_LINE_LENGTH = 100.f;
static const int ROBOT_WIDTH = 8;

class SharedViewerPainter : public tool_common::PaintField
{
    Q_OBJECT;

public:
    SharedViewerPainter(QWidget* parent = 0, float scaleFactor_ = 1.f);

    void updateWithLocationInfo(float xCoord, float yCoord,
                                float heading, float ballDistance, int index);
    void updateWithSharedBallMessage(messages::SharedBall sharedLoc);

//    void updateWithLocationMessage(messages::RobotLocation newLoc);
//    void updateWithSharedBallMessage(messages::SharedBall sharedLoc);
//    void updateWithParticleMessage(messages::ParticleSwarm newSwarm);
//    void updateWithObsvMessage(messages::VisionField newObservations);

//    void updateWithOfflineMessage(messages::RobotLocation newOffline);
//    void updateWithOfflineParticleMessage(messages::ParticleSwarm newOfflineSwarm);
//    void updateWithOfflineObsvMessage(messages::VisionField newObservations);

protected slots:

    void handleZoomIn();
    void handleZoomOut();


protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);

    // Paint a RobotLocation
    void paintRobot(QPaintEvent* event, float x, float y, float h, float ballD,
                    QString playerNum);

    void paintSharedBall(QPaintEvent* event, messages::SharedBall msg);

    float myXCoords[NUM_PLAYERS_PER_TEAM];
    float myYCoords[NUM_PLAYERS_PER_TEAM];
    float myHeadings[NUM_PLAYERS_PER_TEAM];
    float myBallDist[NUM_PLAYERS_PER_TEAM];

private:
//    messages::WorldModel curLoc;
    messages::SharedBall sharedBallLoc;

    man::localization::LineSystem* lineSystem;
    man::localization::VisionSystem* visionSystem;
};

} // namespace shared
} // namespace tool
