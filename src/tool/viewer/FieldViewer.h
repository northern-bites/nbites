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

#include "FieldViewerPainter.h"

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

    portals::InPortal<messages::RobotLocation> locationIn;

protected slots:
    void noLogError();

protected:
    virtual void run_();

protected:
    FieldViewerPainter* fieldPainter;

    QHBoxLayout* mainLayout;
    QHBoxLayout* field;
    QVBoxLayout* checkBoxes;

    QCheckBox* particleViewBox;
    QCheckBox* locationViewBox;

    QCheckBox* selector3;
    QCheckBox* selector4;
    QCheckBox* selector5;
    QCheckBox* selector6;
    QCheckBox* selector7;
    QCheckBox* selector8;


    float scaleFactor;

private:
    bool haveParticleLogs;
    bool haveLocationLogs;
};



}
}
