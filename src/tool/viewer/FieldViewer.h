/**
 * Class responsible for holding a PaintField object and
 * interacting with the user and field (Controller!)
 *
 * @author EJ Googins
 * @date   April 2013
 */
#pragma once

#include <QtGui>
#include <vector>

//#include "PaintField.h"

namespace tool{
namespace viewer{

class FieldViewer : public QWidget
{
    Q_OBJECT;

public:
    FieldViewer(QWidget* parent = 0);

protected:
//    PaintField* fieldPainter;

    QVBoxLayout* mainLayout;
    QHBoxLayout* field;
    float scaleFactor;
};



}
}
