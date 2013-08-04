/*
 * @class VisionSimModule
 *
 * The main module that starts up the model and the GUI; can be added to the
 * Tool's diagram to run on call. This is both a QObject and a Module, so it
 * interfaces between RoboGrams and Qt stuff.
 *
 * @author Lizzie Mamantov
 */

#pragma once

#include <QObject>
#include "RoboGrams.h"
#include "World.h"
#include "Image.h"
#include "VisionSimGUI.h"

namespace tool{

// Forward declaration for friending
class Tool;

namespace visionsim{

class VisionSimModule :  public QObject,
                         public portals::Module
{
    Q_OBJECT;
    // The tool needs to be able to access the gui part
    friend class ::tool::Tool;

public:
    VisionSimModule();
    virtual ~VisionSimModule() {}

public slots:
    // Allow the GUI to update the world model
    void updateRobot(float x, float y, float h);
    void updateHead(float yaw, float pitch);
    void updateBall(float x, float y);

signals:
    // Notify the GUI when the model has changed
    void modelChanged();

protected:
    void updateImages();

    // The required module method
    virtual void run_();

    // Keeps track of the model (world and images) and GUI
    World world;
    Image topImage;
    Image bottomImage;
    VisionSimGUI gui;
};

}
}
