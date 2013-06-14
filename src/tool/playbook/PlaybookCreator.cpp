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

    //GUI
    field = new QHBoxLayout();
    field->addWidget(fieldPainter);

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
    editDefenderX = new QLineEdit("Edit x", this);
    editDefenderY = new QLineEdit("Edit y", this);
    editMiddieX = new QLineEdit("Edit x", this);
    editMiddieY = new QLineEdit("Edit y", this);
    editOffenderX = new QLineEdit("Edit x", this);
    editOffenderY = new QLineEdit("Edit y", this);
    twoFieldPlayers = new QRadioButton("&2 Active Field Players", this);
    threeFieldPlayers = new QRadioButton("&3 Active Field Players", this);
    fourFieldPlayers = new QRadioButton("&4 active Field Players", this);
    fourFieldPlayers->setChecked(true);
    editBallX = new QLineEdit("Ball x", this);
    editBallY = new QLineEdit("Ball y", this);

    settings->addWidget(undoBtn);
    settings->addWidget(loadBtn);
    settings->addWidget(saveBtn);
    settings->addWidget(twoFieldPlayers);
    settings->addWidget(threeFieldPlayers);
    settings->addWidget(fourFieldPlayers);
    settings->addWidget(lockDefender);
    settings->addWidget(editDefenderX);
    settings->addWidget(editDefenderY);
    settings->addWidget(lockMiddie);
    settings->addWidget(editMiddieX);
    settings->addWidget(editMiddieY);
    settings->addWidget(lockOffender);
    settings->addWidget(editOffenderX);
    settings->addWidget(editOffenderY);
    settings->addWidget(goalie);
    settings->addWidget(editBallX);
    settings->addWidget(editBallY);

    // Connect checkbox interface (including disabling lineEdits)
    connect(lockDefender, SIGNAL(toggled(bool)), model,
            SLOT(toggleDefender(bool)));
    connect(lockDefender, SIGNAL(toggled(bool)), editDefenderX,
            SLOT(setDisabled(bool)));
    connect(lockDefender, SIGNAL(toggled(bool)), editDefenderY,
            SLOT(setDisabled(bool)));

    connect(lockOffender, SIGNAL(toggled(bool)), model,
            SLOT(toggleOffender(bool)));
    connect(lockOffender, SIGNAL(toggled(bool)), editOffenderX,
            SLOT(setDisabled(bool)));
    connect(lockOffender, SIGNAL(toggled(bool)), editOffenderY,
            SLOT(setDisabled(bool)));

    connect(lockMiddie, SIGNAL(toggled(bool)), model,
            SLOT(toggleMiddie(bool)));
    connect(lockMiddie, SIGNAL(toggled(bool)), editMiddieX,
            SLOT(setDisabled(bool)));
    connect(lockMiddie, SIGNAL(toggled(bool)), editMiddieY,
            SLOT(setDisabled(bool)));

    connect(goalie, SIGNAL(toggled(bool)), model,
            SLOT(toggleGoalie(bool)));

    connect(goalie, SIGNAL(toggled(bool)), fieldPainter,
            SLOT(drawGoalie(bool)));

    // Connect radio buttons
    connect(twoFieldPlayers, SIGNAL(toggled(bool)), model,
            SLOT(setTwoFieldPlayers(bool)));

    connect(threeFieldPlayers, SIGNAL(toggled(bool)), model,
            SLOT(setThreeFieldPlayers(bool)));

    connect(fourFieldPlayers, SIGNAL(toggled(bool)), model,
            SLOT(setFourFieldPlayers(bool)));

    // Connect line edit widgets' textEdited signals
    connect(editDefenderX, SIGNAL(textEdited(QString)), model,
            SLOT(setDefenderXPosition(QString)));
    connect(editDefenderY, SIGNAL(textEdited(QString)), model,
            SLOT(setDefenderYPosition(QString)));
    connect(editMiddieX, SIGNAL(textEdited(QString)), model,
            SLOT(setMiddieXPosition(QString)));
    connect(editMiddieY, SIGNAL(textEdited(QString)), model,
            SLOT(setMiddieYPosition(QString)));
    connect(editOffenderX, SIGNAL(textEdited(QString)), model,
            SLOT(setOffenderXPosition(QString)));
    connect(editOffenderY, SIGNAL(textEdited(QString)), model,
            SLOT(setOffenderYPosition(QString)));
    connect(editBallX, SIGNAL(textEdited(QString)), model,
            SLOT(setBallX(QString)));
    connect(editBallY, SIGNAL(textEdited(QString)), model,
            SLOT(setBallY(QString)));

    // Connect line edit widgets' editingFinished signals
    connect(editDefenderX, SIGNAL(editingFinished()), this,
            SLOT(refreshTextDefender()));
    connect(editDefenderX, SIGNAL(editingFinished()), model,
            SLOT(resetPositionChanges()));

    connect(editDefenderY, SIGNAL(editingFinished()), this,
            SLOT(refreshTextDefender()));
    connect(editDefenderY, SIGNAL(editingFinished()), model,
            SLOT(resetPositionChanges()));

    connect(editMiddieX, SIGNAL(editingFinished()), this,
            SLOT(refreshTextMiddie()));
    connect(editMiddieX, SIGNAL(editingFinished()), model,
            SLOT(resetPositionChanges()));

    connect(editMiddieY, SIGNAL(editingFinished()), this,
            SLOT(refreshTextMiddie()));
    connect(editMiddieY, SIGNAL(editingFinished()), model,
            SLOT(resetPositionChanges()));

    connect(editOffenderX, SIGNAL(editingFinished()), this,
            SLOT(refreshTextOffender()));
    connect(editOffenderX, SIGNAL(editingFinished()), model,
            SLOT(resetPositionChanges()));

    connect(editOffenderY, SIGNAL(editingFinished()), this,
            SLOT(refreshTextOffender()));
    connect(editOffenderY, SIGNAL(editingFinished()), model,
            SLOT(resetPositionChanges()));

    // Conect line edit widgets' returnPressed signals
    connect(editDefenderX, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));
    connect(editDefenderX, SIGNAL(returnPressed()), model,
            SLOT(confirmPositionChange()));

    connect(editDefenderY, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));
    connect(editDefenderY, SIGNAL(returnPressed()), model,
            SLOT(confirmPositionChange()));

    connect(editMiddieX, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));
    connect(editMiddieX, SIGNAL(returnPressed()), model,
            SLOT(confirmPositionChange()));

    connect(editMiddieY, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));
    connect(editMiddieY, SIGNAL(returnPressed()), model,
            SLOT(confirmPositionChange()));

    connect(editOffenderX, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));
    connect(editOffenderX, SIGNAL(returnPressed()), model,
            SLOT(confirmPositionChange()));

    connect(editOffenderY, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));
    connect(editOffenderY, SIGNAL(returnPressed()), model,
            SLOT(confirmPositionChange()));

    connect(editBallX, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));

    connect(editBallY, SIGNAL(returnPressed()), this,
            SLOT(updatePositions()));
    // Connect radio buttons for update
    connect(twoFieldPlayers, SIGNAL(toggled(bool)), this,
            SLOT(updatePositionsCheck(bool)));
    connect(threeFieldPlayers, SIGNAL(toggled(bool)), this,
            SLOT(updatePositionsCheck(bool)));
    connect(fourFieldPlayers, SIGNAL(toggled(bool)), this,
            SLOT(updatePositionsCheck(bool)));

    mainLayout->addLayout(field);
    mainLayout->addLayout(settings);

    this->setLayout(mainLayout);

    // Initialize data from the model to the fieldPainter.
    updateRobotPositions();
}

void PlaybookCreator::updateRobotPositions()
{
    int fieldPlayers = model->getNumActiveFieldPlayers();
    int i, max;
    short ballX = model->getBallX();
    short ballY = model->getBallY();

    if (fieldPlayers == 4) {
        i = 0;
        max = 3;
    }
    else if (fieldPlayers == 3) {
        i = 3;
        max = 5;
    }
    else if (fieldPlayers == 2) {
        i = 5;
        max = 6;
    }

    for (i; i < max; i++)
    {
        PlaybookPosition* position = model->playbook[model->getGoalieOn()][i][ballX][ballY];

        fieldPainter->setRobot(position,defaultRoleList[i]);
    }

    fieldPainter->setNumActiveFieldPlayers(fieldPlayers);
    fieldPainter->setBallX(ballX);
    fieldPainter->setBallY(ballY);

    fieldPainter->update();
}

void PlaybookCreator::updatePositions()
{
    updateRobotPositions();
    qDebug() << "updating the robot positions now.";
}

void PlaybookCreator::updatePositionsCheck(bool check)
{
    if (check)
    {
        updatePositions();
        refreshTextDefender();
        refreshTextMiddie();
        refreshTextOffender();
    }
}

void PlaybookCreator::refreshTextDefender()
{
    updatePositions();
    editDefenderX->setText(QString::number(fieldPainter->getRobot(DEFENDER)->x));
    editDefenderY->setText(QString::number(fieldPainter->getRobot(DEFENDER)->y));
    qDebug() << "displaying the defender's true position now.";
}

void PlaybookCreator::refreshTextMiddie()
{
    updatePositions();
    editMiddieX->setText(QString::number(fieldPainter->getRobot(MIDDIE)->x));
    editMiddieY->setText(QString::number(fieldPainter->getRobot(MIDDIE)->y));
    qDebug() << "displaying the middie's true position now.";
}

void PlaybookCreator::refreshTextOffender()
{
    updatePositions();
    editOffenderX->setText(QString::number(fieldPainter->getRobot(OFFENDER)->x));
    editOffenderY->setText(QString::number(fieldPainter->getRobot(OFFENDER)->y));
    qDebug() << "displaying the offender's true position now.";
}

} // namespace playbook
} // namespace tool
