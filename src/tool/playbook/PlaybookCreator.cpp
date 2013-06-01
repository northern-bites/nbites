#include "PlaybookCreator.h"

#include <QtDebug>

namespace tool{
namespace playbook{

PlaybookCreator::PlaybookCreator(QWidget* parent):
    QWidget(parent),
    havePlaybook(false)
{
    model = new PlaybookModel(this);

    fieldPainter = new PlaybookField(this);

    mainLayout = new QHBoxLayout(this);

    //GUI
    field = new QHBoxLayout();
    field->addWidget(fieldPainter);

    settings = new QVBoxLayout();
    settings->setAlignment(Qt::AlignTop);

    undoBtn = new QPushButton("Undo", this);
    loadBtn = new QPushButton("Load", this);
    saveBtn = new QPushButton("Save", this);
    lockDefender = new QCheckBox("Lock Defender", this);
    lockMiddie = new QCheckBox("Lock Middie", this);
    lockOffender = new QCheckBox("Lock Offender", this);
    goalie = new QCheckBox("Goalie Active", this);
    goalie->setChecked(true);

    settings->addWidget(undoBtn);
    settings->addWidget(loadBtn);
    settings->addWidget(saveBtn);
    settings->addWidget(lockDefender);
    settings->addWidget(lockMiddie);
    settings->addWidget(lockOffender);
    settings->addWidget(goalie);

    // Connect checkbox interface with slots in the painter
    connect(goalie, SIGNAL(toggled(bool)), model,
            SLOT(toggleGoalie(bool)));
    connect(goalie, SIGNAL(toggled(bool)), fieldPainter,
            SLOT(drawGoalie(bool)));

    mainLayout->addLayout(field);
    mainLayout->addLayout(settings);

    this->setLayout(mainLayout);
}

} // namespace playbook
} // namespace tool
