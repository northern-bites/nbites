/*
 * Main front for the program.
 */

#pragma once

#include "ImageView.h"
#include "WorldView.h"
#include "Controls.h"

namespace tool{
namespace visionsim{

// Forward declaration for friending
class VisionSimModule;

class VisionSimGUI  : public QWidget {
    Q_OBJECT;
    friend class VisionSimModule;

public:
    VisionSimGUI(Image& top, Image& bottom, World& world,
                 QWidget* parent = 0);
    ~VisionSimGUI() {};

public slots:
    void updateView();

private:
    ImageView topImageView;
    ImageView bottomImageView;
    WorldView worldView;
    Controls controls;
};

}
}
