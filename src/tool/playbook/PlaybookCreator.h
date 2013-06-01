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

class PlaybookCreator : public QWidget
{
    Q_OBJECT;

public:
    PlaybookCreator(QWidget* parent = 0);

protected:
    PlaybookField* fieldPainter;
    PlaybookModel* model;

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
