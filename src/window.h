#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QGridLayout>
#include "RoboImage.h"


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
    QLabel *infoLabel;
    QComboBox *shapeComboBox;
    RoboImage r;


};

#endif // WINDOW_H
