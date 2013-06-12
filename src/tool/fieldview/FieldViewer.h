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

    portals::InPortal<messages::RobotLocation> locationIn;
    portals::InPortal<messages::RobotLocation> odometryIn;
    portals::InPortal<messages::ParticleSwarm> particlesIn;
    portals::InPortal<messages::VisionField> observationsIn;

    portals::OutPortal<messages::RobotLocation> odometry;
    portals::OutPortal<messages::VisionField>   observations;
    portals::InPortal <messages::RobotLocation> offline;

protected slots:
    void noLogError();

protected:
    virtual void run_();

protected:
    FieldViewerPainter* fieldPainter;

    QVBoxLayout* mainLayout;
    QHBoxLayout* field;
    QHBoxLayout* checkBoxes;
    QHBoxLayout* resizeLayout;

    QCheckBox* particleViewBox;
    QCheckBox* locationViewBox;
    QCheckBox* robotFieldViewBox;

    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;

    float scaleFactor;

private:
    man::localization::LocalizationModule locMod;
    portals::RoboGram subdiagram;

    bool haveParticleLogs;
    bool haveLocationLogs;
    bool haveVisionFieldLogs;
    bool haveOdometryLogs;
};



}
}
