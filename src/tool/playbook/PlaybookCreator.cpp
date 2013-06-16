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
    editDefenderH = new QLineEdit("Edit h", this);
    editMiddieX = new QLineEdit("Edit x", this);
    editMiddieY = new QLineEdit("Edit y", this);
    editMiddieH = new QLineEdit("Edit h", this);
    editOffenderX = new QLineEdit("Edit x", this);
    editOffenderY = new QLineEdit("Edit y", this);
    editOffenderH = new QLineEdit("Edit h", this);
    twoFieldPlayers = new QRadioButton("&2 Active Field Players", this);
    threeFieldPlayers = new QRadioButton("&3 Active Field Players", this);
    fourFieldPlayers = new QRadioButton("&4 active Field Players", this);
    fourFieldPlayers->setChecked(true);
    editBallX = new QLineEdit("Ball x", this);
    editBallY = new QLineEdit("Ball y", this);

    QGroupBox* defenderBox = new QGroupBox("Defender");
    QGroupBox* middieBox = new QGroupBox("Middie");
    QGroupBox* offenderBox = new QGroupBox("Offender");
    QGroupBox* ballBox = new QGroupBox("Ball");

    QVBoxLayout* defenderBoxLayout = new QVBoxLayout(defenderBox);
    QVBoxLayout* middieBoxLayout = new QVBoxLayout(middieBox);
    QVBoxLayout* offenderBoxLayout = new QVBoxLayout(offenderBox);
    QVBoxLayout* ballBoxLayout = new QVBoxLayout(ballBox);

    defenderBoxLayout->addWidget(lockDefender);
    defenderBoxLayout->addWidget(editDefenderX);
    defenderBoxLayout->addWidget(editDefenderY);
    defenderBoxLayout->addWidget(editDefenderH);
    middieBoxLayout->addWidget(lockMiddie);
    middieBoxLayout->addWidget(editMiddieX);
    middieBoxLayout->addWidget(editMiddieY);
    middieBoxLayout->addWidget(editMiddieH);
    offenderBoxLayout->addWidget(lockOffender);
    offenderBoxLayout->addWidget(editOffenderX);
    offenderBoxLayout->addWidget(editOffenderY);
    offenderBoxLayout->addWidget(editOffenderH);
    ballBoxLayout->addWidget(editBallX);
    ballBoxLayout->addWidget(editBallY);

    settings->addWidget(undoBtn);
    settings->addWidget(loadBtn);
    settings->addWidget(saveBtn);
    settings->addWidget(twoFieldPlayers);
    settings->addWidget(threeFieldPlayers);
    settings->addWidget(fourFieldPlayers);
    settings->addWidget(goalie);
    settings->addWidget(defenderBox);
    settings->addWidget(middieBox);
    settings->addWidget(offenderBox);
    settings->addWidget(ballBox);

    // Connect checkbox interface (including disabling lineEdits)
    connect(lockDefender, SIGNAL(toggled(bool)), model,
            SLOT(toggleDefender(bool)));
    connect(lockDefender, SIGNAL(toggled(bool)), editDefenderX,
            SLOT(setDisabled(bool)));
    connect(lockDefender, SIGNAL(toggled(bool)), editDefenderY,
            SLOT(setDisabled(bool)));
    connect(lockDefender, SIGNAL(toggled(bool)), editDefenderH,
            SLOT(setDisabled(bool)));

    connect(lockOffender, SIGNAL(toggled(bool)), model,
            SLOT(toggleOffender(bool)));
    connect(lockOffender, SIGNAL(toggled(bool)), editOffenderX,
            SLOT(setDisabled(bool)));
    connect(lockOffender, SIGNAL(toggled(bool)), editOffenderY,
            SLOT(setDisabled(bool)));
    connect(lockOffender, SIGNAL(toggled(bool)), editOffenderH,
            SLOT(setDisabled(bool)));

    connect(lockMiddie, SIGNAL(toggled(bool)), model,
            SLOT(toggleMiddie(bool)));
    connect(lockMiddie, SIGNAL(toggled(bool)), editMiddieX,
            SLOT(setDisabled(bool)));
    connect(lockMiddie, SIGNAL(toggled(bool)), editMiddieY,
            SLOT(setDisabled(bool)));
    connect(lockMiddie, SIGNAL(toggled(bool)), editMiddieH,
            SLOT(setDisabled(bool)));

    connect(goalie, SIGNAL(toggled(bool)), model,
            SLOT(toggleGoalie(bool)));

    connect(goalie, SIGNAL(toggled(bool)), fieldPainter,
            SLOT(drawGoalie(bool)));

    // Connect radio buttons
    connect(twoFieldPlayers, SIGNAL(toggled(bool)), this,
            SLOT(setTwoFieldPlayers(bool)));
    connect(threeFieldPlayers, SIGNAL(toggled(bool)), this,
            SLOT(setThreeFieldPlayers(bool)));
    connect(fourFieldPlayers, SIGNAL(toggled(bool)), this,
            SLOT(setFourFieldPlayers(bool)));

    // Connect line edit widgets' editingFinished signals
    connect(editDefenderX, SIGNAL(editingFinished()), this,
            SLOT(refreshTextDefender()));
    connect(editDefenderY, SIGNAL(editingFinished()), this,
            SLOT(refreshTextDefender()));
    connect(editDefenderH, SIGNAL(editingFinished()), this,
            SLOT(refreshTextDefender()));

    connect(editMiddieX, SIGNAL(editingFinished()), this,
            SLOT(refreshTextMiddie()));
    connect(editMiddieY, SIGNAL(editingFinished()), this,
            SLOT(refreshTextMiddie()));
    connect(editMiddieH, SIGNAL(editingFinished()), this,
            SLOT(refreshTextMiddie()));

    connect(editOffenderX, SIGNAL(editingFinished()), this,
            SLOT(refreshTextOffender()));
    connect(editOffenderY, SIGNAL(editingFinished()), this,
            SLOT(refreshTextOffender()));
    connect(editOffenderH, SIGNAL(editingFinished()), this,
            SLOT(refreshTextOffender()));

    connect(editBallX, SIGNAL(editingFinished()), this,
            SLOT(refreshTextBall()));
    connect(editBallY, SIGNAL(editingFinished()), this,
            SLOT(refreshTextBall()));

    // Conect line edit widgets' returnPressed signals
    connect(editDefenderX, SIGNAL(returnPressed()), this,
            SLOT(setDefenderXPosition()));
    connect(editDefenderY, SIGNAL(returnPressed()), this,
            SLOT(setDefenderYPosition()));
    connect(editDefenderH, SIGNAL(returnPressed()), this,
            SLOT(setDefenderHPosition()));

    connect(editMiddieX, SIGNAL(returnPressed()), this,
            SLOT(setMiddieXPosition()));
    connect(editMiddieY, SIGNAL(returnPressed()), this,
            SLOT(setMiddieYPosition()));
    connect(editMiddieH, SIGNAL(returnPressed()), this,
            SLOT(setMiddieHPosition()));

    connect(editOffenderX, SIGNAL(returnPressed()), this,
            SLOT(setOffenderXPosition()));
    connect(editOffenderY, SIGNAL(returnPressed()), this,
            SLOT(setOffenderYPosition()));
    connect(editOffenderH, SIGNAL(returnPressed()), this,
            SLOT(setOffenderHPosition()));

    connect(editBallX, SIGNAL(returnPressed()), this,
            SLOT(setBallX()));
    connect(editBallY, SIGNAL(returnPressed()), this,
            SLOT(setBallY()));

    mainLayout->addLayout(field);
    mainLayout->addLayout(settings);

    this->setLayout(mainLayout);

    // Initialize data from the model to the fieldPainter.
    updateRobotPositions();
    // Refresh all of the text fields.
    refreshTextAll();
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
        PlaybookPosition* position = model->playbook[model->getGoalieOn()][ballX][ballY][i];

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
    editDefenderH->setText(QString::number(fieldPainter->getRobot(DEFENDER)->h));
    qDebug() << "displaying the defender's true position now.";
}

void PlaybookCreator::refreshTextMiddie()
{
    updatePositions();
    editMiddieX->setText(QString::number(fieldPainter->getRobot(MIDDIE)->x));
    editMiddieY->setText(QString::number(fieldPainter->getRobot(MIDDIE)->y));
    editMiddieH->setText(QString::number(fieldPainter->getRobot(MIDDIE)->h));
    qDebug() << "displaying the middie's true position now.";
}

void PlaybookCreator::refreshTextOffender()
{
    updatePositions();
    editOffenderX->setText(QString::number(fieldPainter->getRobot(OFFENDER)->x));
    editOffenderY->setText(QString::number(fieldPainter->getRobot(OFFENDER)->y));
    editOffenderH->setText(QString::number(fieldPainter->getRobot(OFFENDER)->h));
    qDebug() << "displaying the offender's true position now.";
}

void PlaybookCreator::refreshTextBall()
{
    updatePositions();
    editBallX->setText(QString::number(model->getBallX()));
    editBallY->setText(QString::number(model->getBallY()));
    qDebug() << "displaying the ball's true position now.";
}

void PlaybookCreator::refreshTextAll()
{
    refreshTextDefender();
    refreshTextMiddie();
    refreshTextOffender();
    refreshTextBall();
}

void PlaybookCreator::setTwoFieldPlayers(bool checked)
{
    if (checked)
    {
        model->setNumActiveFieldPlayers(2);
        qDebug() << "Number of field players is now 2.";

        updateLockedPositions();
        refreshTextAll();
    }
}

void PlaybookCreator::setThreeFieldPlayers(bool checked)
{
    if (checked)
    {
        model->setNumActiveFieldPlayers(3);
        qDebug() << "Number of field players is now 3.";

        updateLockedPositions();
        refreshTextAll();
    }
}

void PlaybookCreator::setFourFieldPlayers(bool checked)
{
    if (checked)
    {
        model->setNumActiveFieldPlayers(4);
        qDebug() << "Number of field players is now 4.";

        updateLockedPositions();
        refreshTextAll();
    }
}

void PlaybookCreator::updateLockedPositions()
{
    // Check if any players are locked
    if (model->getDefenderLocked())
    {
        setDefenderXPosition();
        setDefenderYPosition();
        setDefenderHPosition();
    }
    if (model->getMiddieLocked())
    {
        setMiddieXPosition();
        setMiddieYPosition();
        setMiddieHPosition();
    }
    if (model->getOffenderLocked())
    {
        setOffenderXPosition();
        setOffenderYPosition();
        setOffenderHPosition();
    }
}

void PlaybookCreator::setDefenderXPosition()
{
    model->setDefenderXPosition(editDefenderX->text().toInt());
}

void PlaybookCreator::setDefenderYPosition()
{
    model->setDefenderYPosition(editDefenderY->text().toInt());
}

void PlaybookCreator::setDefenderHPosition()
{
    model->setDefenderHPosition(editDefenderH->text().toInt());
}

void PlaybookCreator::setMiddieXPosition()
{
    model->setMiddieXPosition(editMiddieX->text().toInt());
}

void PlaybookCreator::setMiddieYPosition()
{
    model->setMiddieYPosition(editMiddieY->text().toInt());
}

void PlaybookCreator::setMiddieHPosition()
{
    model->setMiddieHPosition(editMiddieH->text().toInt());
}

void PlaybookCreator::setOffenderXPosition()
{
    model->setOffenderXPosition(editOffenderX->text().toInt());
}

void PlaybookCreator::setOffenderYPosition()
{
    model->setOffenderYPosition(editOffenderY->text().toInt());
}

void PlaybookCreator::setOffenderHPosition()
{
    model->setOffenderHPosition(editOffenderH->text().toInt());
}

void PlaybookCreator::setBallX()
{
    model->setBallX(editBallX->text().toInt());
}

void PlaybookCreator::setBallY()
{
    model->setBallY(editBallY->text().toInt());
}

} // namespace playbook
} // namespace tool
