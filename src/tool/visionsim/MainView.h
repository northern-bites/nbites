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
    void updateRobot(float x, float y, float h);
    void updateHead(float yaw, float pitch);
    void updateBall(float x, float y);

private:
    // Model
    World world;
    Image topImage;
    Image bottomImage;

    // View
    ImageView topImageView;
    ImageView bottomImageView;
    WorldView worldView;
    Controls controls;

    void updateView();
};

class MainView : public QMainWindow {
Q_OBJECT

public:
    MainView();
    ~MainView() {};
};
