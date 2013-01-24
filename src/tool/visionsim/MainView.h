/*
 * Main front for the program.
 */

#pragma once

#include <QMainWindow>
#include "ImageView.h"
#include "WorldView.h"
#include "Controls.h"

class MainWidget : public QWidget {
Q_OBJECT

public:
    MainWidget(QWidget* parent = 0);
    ~MainWidget() {};

public slots:
    void updateView();

private:
    ImageView topImageView;
    ImageView bottomImageView;
    WorldView worldView;
    Controls controls;

};

class MainView : public QMainWindow {
Q_OBJECT

public:
    MainView();
    ~MainView() {};
};
