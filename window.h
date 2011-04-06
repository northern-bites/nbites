#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QGridLayout>
#include "roboimage.h"


class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;
class RenderArea;

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(RoboImage r1, QWidget *parent = 0);
private slots:
    void shapeChanged();

private:
    RenderArea *renderArea;
    QLabel *shapeLabel;
    QComboBox *shapeComboBox;
    RoboImage r;


};

#endif // WINDOW_H
