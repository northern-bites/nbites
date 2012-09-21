/*
 * Main front for the program.
 */

#pragma once

#include <QMainWindow>
#include "../model/Model.h"
#include "ImageView.h"

class MainWidget;

class MainView : public QMainWindow {

Q_OBJECT

public:
    MainView();
    ~MainView() {};
};

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
};
