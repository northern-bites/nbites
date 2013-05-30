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

namespace tool{
namespace playbook{

class PlaybookCreator : public QWidget
{
    Q_OBJECT;

public:
    PlaybookCreator(QWidget* parent = 0);

protected slots:
    void noPlaybookError();

protected:
    PlaybookField* fieldPainter;

    QHBoxLayout* mainLayout;
    QHBoxLayout* field;
    QVBoxLayout* settings;

    QCheckBox* lockDefender;
    QCheckBox* lockMiddie;
    QCheckBox* lockOffender;
    QCheckBox* goalie;

private:
    bool havePlaybook;
};

}
}
