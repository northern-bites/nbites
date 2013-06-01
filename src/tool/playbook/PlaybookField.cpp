#include "PlaybookField.h"

#include <QtDebug>

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

void PlaybookField::drawGoalie(bool on)
{
    qDebug() << "Drawing goalie " << on;
}

} // namespace playbook
} // namespace tool
