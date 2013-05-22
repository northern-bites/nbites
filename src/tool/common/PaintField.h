/**
 *  This class draws a field so we can display movements
 *  of robots and the ball on the field visually. Run method in case
 *  some one wants to make it a module and run it in a diagram
 *  Can also inherit from to make specific painters
 *
 *  @author EJ Googins
 *  @date April 2013
 */

#pragma once

#include <QtGui>
#include <vector>

#include "common/FieldConstants.h"

namespace tool{
namespace viewer{

class PaintField : public QWidget
{
    Q_OBJECT;

public:
    PaintField(QWidget* parent = 0, float scaleFactor_ = 1.f);

protected:
    virtual void run_();

    // Keeps the widget the right size
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // Paint the field
    void paintEvent(QPaintEvent* event);

protected:
    float scaleFactor;
};


} //namespace tool
} //namespace viewer
