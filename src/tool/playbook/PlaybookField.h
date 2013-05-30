/**
 * Class responsible for drawing more things on the field,
 * inherits from PaintField so that the field itself is already drawn.
 * Based on specs from the PlaybookCreator Class
 *
 * @author Wils Dawson May 2013
 *
 */

#pragma once

#include <QtGui>
#include <vector>

#include "common/FieldConstants.h"

#include "common/PaintField.h"

namespace tool {
namespace playbook {

static const int PARTICLE_WIDTH = 8;

class PlaybookField : public PaintField
{
    Q_OBJECT;

public:
    PlaybookField(QWidget* parent = 0, float scaleFactor_ = 1.f);

protected slots:

protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);

private:

};

} // namespace playbook
} // namespace tool
