#include "PlaybookField.h"

namespace tool {
namespace playbook {

PlaybookField::PlaybookField(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_)
{
}

void PlaybookField::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);
}

} // namespace playbook
} // namespace tool
