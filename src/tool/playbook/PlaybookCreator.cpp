#include "PlaybookCreator.h"

#include <QtDebug>

namespace tool{
namespace playbook{

PlaybookCreator::PlaybookCreator(QWidget* parent):
    QWidget(parent),
    havePlaybook(false)
{
    model = new PlaybookModel(BOX_SIZE, GRID_WIDTH, GRID_HEIGHT, this);

    fieldPainter = new PlaybookField(BOX_SIZE, GRID_WIDTH, GRID_HEIGHT, this);

    mainLayout = new QHBoxLayout(this);

    graphicsScene = new QGraphicsScene();
    QGraphicsView graphicsView(graphicsScene);

    graphicsView.setBackgroundBrush(QColor(230, 200, 167));

    //GUI
    field = new QHBoxLayout();
    //field->addWidget(fieldPainter);
    field->addWidget(&graphicsView);

    settings = new QVBoxLayout();
    settings->setAlignment(Qt::AlignTop);

    undoBtn = new QPushButton("Undo", this);
    loadBtn = new QPushButton("Load", this);
    saveBtn = new QPushButton("Save", this);
    lockDefender = new QCheckBox("Lock Defender", this);
    lockOffender = new QCheckBox("Lock Offender", this);
    lockMiddie = new QCheckBox("Lock Middie", this);
    goalie = new QCheckBox("Goalie Active", this);
    goalie->setChecked(true);

    settings->addWidget(undoBtn);
    settings->addWidget(loadBtn);
    settings->addWidget(saveBtn);
    settings->addWidget(lockDefender);
    settings->addWidget(lockMiddie);
    settings->addWidget(lockOffender);
    settings->addWidget(goalie);

    // Connect checkbox interface
    connect(lockDefender, SIGNAL(toggled(bool)), model,
            SLOT(toggleDefender(bool)));

    connect(lockOffender, SIGNAL(toggled(bool)), model,
            SLOT(toggleOffender(bool)));

    connect(lockMiddie, SIGNAL(toggled(bool)), model,
            SLOT(toggleMiddie(bool)));

    connect(goalie, SIGNAL(toggled(bool)), model,
            SLOT(toggleGoalie(bool)));
    connect(goalie, SIGNAL(toggled(bool)), fieldPainter,
            SLOT(drawGoalie(bool)));

    mainLayout->addLayout(field);
    mainLayout->addLayout(settings);

    this->setLayout(mainLayout);

    // Initialize data from the model to the fieldPainter.
    for (int i = 0; i < 3; i++)
    {
        PlaybookPosition* position = model->playbook[0][i][0][0];
        RobotGraphics* robot = new RobotGraphics(position->x, position->y,
                                                position->h, position->role,
                                                roleColors[position->role]);
        fieldPainter->setRobot(robot,i);
        robot->setPos(100,500);
        graphicsScene->addItem(robot);
    }
}

} // namespace playbook
} // namespace tool
