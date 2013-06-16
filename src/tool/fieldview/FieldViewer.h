/**
 * Class responsible for holding a PaintField object and
 * interacting with the user and field (Controller!)
 *
 * @author EJ Googins
 * @date   April 2013
 */
#pragma once

#include <QtGui>
#include <QCheckBox>
#include <vector>

#include "RoboGrams.h"
#include "RobotLocation.pb.h"
#include "ParticleSwarm.pb.h"

#include "FieldViewerPainter.h"
#include "localization/LocalizationModule.h"
#include "OfflineLocListener.h"

namespace tool{
namespace viewer{

class FieldViewer : public QWidget,
                    public portals::Module
{
    Q_OBJECT;

public:
    FieldViewer(QWidget* parent = 0);

    void confirmParticleLogs(bool haveLogs);
    void confirmLocationLogs(bool haveLogs);
    void confirmObsvLogs(bool haveLogs);
    void confirmOdometryLogs(bool haveLogs);

    void tryOffline();

    portals::InPortal<messages::RobotLocation> locationIn;
    portals::InPortal<messages::RobotLocation> odometryIn;
    portals::InPortal<messages::ParticleSwarm> particlesIn;
    portals::InPortal<messages::VisionField> observationsIn;

    portals::OutPortal<messages::RobotLocation> odometry;
    portals::OutPortal<messages::VisionField>   observations;

    portals::InPortal <messages::RobotLocation> offline;
    portals::InPortal <messages::ParticleSwarm> offlineSwarm;

protected slots:
    void noLogError();

protected:
    virtual void run_();

protected:
    FieldViewerPainter* fieldPainter;

    QVBoxLayout* mainLayout;
    QHBoxLayout* field;
    QHBoxLayout* checkBoxes;
    QHBoxLayout* checkBoxesOffline;
    QHBoxLayout* resizeLayout;

    QCheckBox* particleViewBox;
    QCheckBox* locationViewBox;
    QCheckBox* robotFieldViewBox;
    QCheckBox* particleViewBoxOffline;
    QCheckBox* locationViewBoxOffline;
    QCheckBox* robotFieldViewBoxOffline;

    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;

    float scaleFactor;

private:
    man::localization::LocalizationModule locMod;
    tool::OfflineLocListener locListen;
    portals::RoboGram subdiagram;

    bool haveParticleLogs;
    bool haveLocationLogs;
    bool haveVisionFieldLogs;
    bool haveOdometryLogs;
};



}
}
