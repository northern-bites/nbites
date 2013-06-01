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

    undoBtn = new QPushButton("Undo", this);
    loadBtn = new QPushButton("Load", this);
    saveBtn = new QPushButton("Save", this);
    lockDefender = new QCheckBox("Lock Defender", this);
    lockMiddie = new QCheckBox("Lock Middie", this);
    lockOffender = new QCheckBox("Lock Offender", this);
    goalie = new QCheckBox("Goalie Inactive", this);

    settings->addWidget(undoBtn);
    settings->addWidget(loadBtn);
    settings->addWidget(saveBtn);
    settings->addWidget(lockDefender);
    settings->addWidget(lockMiddie);
    settings->addWidget(lockOffender);
    settings->addWidget(goalie);

    // Connect checkbox interface with slots in the painter
    // Assume no unloggers, hookup if proven wrong
    //connect(robotFieldViewBox, SIGNAL(toggled(bool)), this,
    //        SLOT(noLogError()));

    mainLayout->addLayout(field);
    mainLayout->addLayout(settings);

    this->setLayout(mainLayout);
}

// void FieldViewer::confirmObsvLogs(bool haveLogs)
// {
//     haveVisionFieldLogs = haveLogs;
//     if(haveLogs) {
//         connect(robotFieldViewBox, SIGNAL(toggled(bool)), fieldPainter,
//                 SLOT(paintObsvAction(bool)));

//         disconnect(robotFieldViewBox, SIGNAL(toggled(bool)), this,
//                    SLOT(noLogError()));
//     }
//     else {
//         disconnect(robotFieldViewBox, SIGNAL(toggled(bool)), fieldPainter,
//                 SLOT(paintObsvAction(bool)));

//         connect(robotFieldViewBox, SIGNAL(toggled(bool)), this,
//                    SLOT(noLogError()));
//     }
// }

void PlaybookCreator::noPlaybookError()
{
    qDebug() << "Sorry, the opened log file does not include those logs";
}

} // namespace playbook
} // namespace tool
