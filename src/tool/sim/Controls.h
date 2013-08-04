/*
 * @class Controls
 *
 * The Controls class provides a GUI for moving a robot and ball around
 * a field. This involves lots of spin boxes so that the user can type
 * or click to a certain destination.
 *
 * @author Lizzie Mamantov
 *
 */

#pragma once

#include <QSpinBox>

namespace tool{
namespace visionsim{

class Controls : public QWidget {
Q_OBJECT

public:
    Controls(QWidget* parent = 0);
    ~Controls() {};

signals:
    // Linked to the model classes that need to hear from the GUI
    void robotMoved(float x, float y, float h);
    void headMoved(float yaw, float pitch);
    void ballMoved(float x, float y);

private slots:
    // Take care of sending information
    void sendRobotInfo();
    void sendHeadInfo();
    void sendBallInfo();

private:
    // The various spin boxes used to control values
    QSpinBox robotX;
    QSpinBox robotY;
    QSpinBox robotH;
    QSpinBox headYaw;
    QSpinBox headPitch;
    QSpinBox ballX;
    QSpinBox ballY;
};

}
}
