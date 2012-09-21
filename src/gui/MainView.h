/*
 * Main front for the program.
 */

#pragma once

#include <QMainWindow>
#include "ImageView.h"
#include "WorldView.h"

class MainWidget : public QWidget {
Q_OBJECT

public:
    MainWidget(QWidget* parent = 0);
    ~MainWidget() {};

private:
    // Model
    World world;
    Image topImage;
    Image bottomImage;

    // View
    ImageView topImageView;
    ImageView bottomImageView;
    WorldView worldView;
};

class MainView : public QMainWindow {
Q_OBJECT

public:
    MainView();
    ~MainView() {};
};
