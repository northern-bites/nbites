#include "FieldViewer.h"

namespace tool{
namespace viewer{

FieldViewer::FieldViewer(QWidget* parent):
    QWidget(parent)
{
//    fieldPainter(this);

    mainLayout = new QVBoxLayout(this);

    field = new QHBoxLayout();
//    field->addWidget(fieldPainter);

    mainLayout->addLayout(field);
    this->setLayout(mainLayout);
}

} // namespace viewer
} // namespace tool
