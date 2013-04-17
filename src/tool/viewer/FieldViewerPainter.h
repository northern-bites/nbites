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

#include "FieldConstants.h"

#include "common/PaintField.h"

namespace tool {
namespace viewer {

class FieldViewerPainter : public PaintField
{
    Q_OBJECT;

public:
    FieldViewerPainter(QWidget* parent = 0, float scaleFactor_ = 1.f);

};

} // namespace viewer
} // namespace tool
