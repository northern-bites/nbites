#include "FieldViewer.h"

#include <QtDebug>

namespace tool{
namespace viewer{

FieldViewer::FieldViewer(QWidget* parent):
    QWidget(parent)
{
    fieldPainter = new PaintField(this);

    mainLayout = new QHBoxLayout(this);

    particleViewBox = new QCheckBox("Particle Viewer",this);
    selector2 = new QCheckBox("test2", this);
    selector3 = new QCheckBox("test3",this);
    selector4 = new QCheckBox("test4", this);
    selector5= new QCheckBox("test5",this);
    selector6 = new QCheckBox("test6", this);
    selector7 = new QCheckBox("test7",this);
    selector8 = new QCheckBox("test8", this);

    field = new QHBoxLayout();
    field->addWidget(fieldPainter);

    checkBoxes = new QVBoxLayout();

    checkBoxes->addWidget(particleViewBox);
    checkBoxes->addWidget(selector2);
    checkBoxes->addWidget(selector3);
    checkBoxes->addWidget(selector4);
    checkBoxes->addWidget(selector5);
    checkBoxes->addWidget(selector6);
    checkBoxes->addWidget(selector7);
    checkBoxes->addWidget(selector8);

    connect(particleViewBox, SIGNAL(toggled(bool)), this,
            SLOT(paintParticleView(bool)));

    mainLayout->addLayout(field);
    mainLayout->addLayout(checkBoxes);

    this->setLayout(mainLayout);
}

void FieldViewer::paintParticleView(bool state) {
    if (state)
        qDebug() << "Paint Particle View";
    else
        qDebug() << "Don't Paint Particle View";
}

} // namespace viewer
} // namespace tool
