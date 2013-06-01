/**
 * Class responsible for drawing more things on the field,
 * inherits from PaintField so that the field itself is already drawn.
 * Based on specs from the PlaybookCreator Class
 *
 * @author Wils Dawson
 * @date May 2013
 */

#pragma once

#include <QtGui>
#include <vector>

#include "FieldConstants.h"

#include "common/PaintField.h"

namespace tool {
namespace playbook {

class PlaybookField : public PaintField
{
    Q_OBJECT;

public:
    PlaybookField(QWidget* parent = 0, float scaleFactor_ = 1.f);

protected slots:
    void drawGoalie(bool on);

protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);

private:

};

} // namespace playbook
} // namespace tool
