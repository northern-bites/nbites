/*
 * @class VisionSimGUI
 *
 * Main GUI for the simulator; can be added to the QTool as a tab widget.
 * Provides image viewers, a control panel, and a world view so that the
 * model can be controlled and the results viewed in the projected images.
 *
 * @author Lizzie Mamantov
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
    // Tells the different viewers to update themselves
    void updateView();

private:
    ImageView topImageView;
    ImageView bottomImageView;
    WorldView worldView;
    Controls controls;
};

}
}
