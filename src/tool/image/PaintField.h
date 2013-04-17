/**
 *  This class draws a field so we can display movements
 *  of robots and the ball on the field visually.
 *
 *  @author EJ Googins
 *  @date April 2013
 */

#pragma once

#include <QtGui>
#include <vector>

#include "FieldConstants.h"

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

private:
    QLabel* authorName;
};


} //namespace tool
} //namespace viewer
