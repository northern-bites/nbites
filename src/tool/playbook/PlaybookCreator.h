/**
 * Class responsible for creating, viewing, and editing a playbook.
 *
 * @author Wils Dawson
 * @date   May 2013
 */
#pragma once

#include <QtGui>
#include <QCheckBox>
#include <vector>

#include "PlaybookField.h"
#include "PlaybookModel.h"

namespace tool{
namespace playbook{

static const int BOX_SIZE = 100;  // Size in cm for each grid box.

static const int GRID_WIDTH  = (int(FIELD_WHITE_WIDTH) / BOX_SIZE) + 2;
static const int GRID_HEIGHT = (int(FIELD_WHITE_HEIGHT)/ BOX_SIZE) + 2;

class PlaybookCreator : public QWidget
{
    Q_OBJECT;

public:
    PlaybookCreator(QWidget* parent = 0);

protected:
    PlaybookField* fieldPainter;
    PlaybookModel* model;
    QGraphicsScene* graphicsScene;

    QHBoxLayout* mainLayout;
    QHBoxLayout* field;
    QVBoxLayout* settings;

    QPushButton* undoBtn;
    QPushButton* loadBtn;
    QPushButton* saveBtn;
    QCheckBox* lockDefender;
    QCheckBox* lockMiddie;
    QCheckBox* lockOffender;
    QCheckBox* goalie;

private:
    bool havePlaybook;
};

}
}
