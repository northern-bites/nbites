/*
 * Main front for the program.
 */

#pragma once

#include "ImageView.h"
#include "WorldView.h"
#include "Controls.h"

namespace tool{
namespace visionsim{

class VisionSimGUI  : public QWidget {
    Q_OBJECT;

public:
    VisionSimGUI(QWidget* parent = 0);
    ~VisionSimGUI() {};

public slots:
    void updateView();

private:
    // ImageView topImageView;
    // ImageView bottomImageView;
    // WorldView worldView;
    Controls controls;
};

}
}
