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

//#include "common/PaintField.h"
#include "FieldViewerPainter.h"

namespace tool{
namespace viewer{

class FieldViewer : public QWidget
{
    Q_OBJECT;

public:
    FieldViewer(QWidget* parent = 0);

protected slots:
    void paintParticleView(bool state);

protected:
    FieldViewerPainter* fieldPainter;

    QHBoxLayout* mainLayout;
    QHBoxLayout* field;
    QVBoxLayout* checkBoxes;

    QCheckBox* particleViewBox;
    QCheckBox* selector2;
    QCheckBox* selector3;
    QCheckBox* selector4;
    QCheckBox* selector5;
    QCheckBox* selector6;
    QCheckBox* selector7;
    QCheckBox* selector8;


    float scaleFactor;
};



}
}
